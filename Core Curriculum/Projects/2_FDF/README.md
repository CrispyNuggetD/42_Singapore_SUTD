*This project has been created as part of the 42 curriculum by hnah.*

# FdF — from wireframe to multiplayer testbed

## Description

FdF reads a `.fdf` height map and renders it as a fitted isometric wireframe
with MiniLibX. Each number in the file becomes a 3D point `(x, y, z)`. The
program projects those points onto the screen and joins horizontal and vertical
neighbours with colour-aware rasterised lines.

The mandatory program is deliberately small: load, project, fit, draw, display,
and clean up.

The bonus grew into an experiment rather than a real game. It adds rotating
and movable isometric projection, zoom, two movable points, two first-person
cameras, and a way for a second terminal (including one on another computer)
to provide controls.
There is no server inside FdF, no game protocol, no synchronised world on two
machines, and no attempt at production networking. `nc` merely transports raw
keyboard bytes into the FdF process's standard input. I therefore call it a
**multiplayer testbed**, not a multiplayer game.

## The rendering pipeline

The mandatory render follows this path:

1. `map_dimensions.c` validates that the map is rectangular and obtains its
   width and height.
2. `map_load.c` allocates one flat point array and loads height and optional
   colour values. Point `(x, y)` is stored at `y * width + x`.
3. `project_model()` centres the map around its midpoint and applies the
   isometric equations. The same function can first rotate the model around
   the Z axis, which lets the bonus extend the mandatory renderer without
   duplicating it.
4. `find_bounds()` projects every point once to find the actual projected
   extents. `fit_projection()` chooses the smaller horizontal/vertical scale
   and centres the result with a margin. This is why maps of very different
   dimensions still fit the window.
5. `render_map()` projects every point again and connects only its right and
   bottom neighbours. That draws every grid edge exactly once.
6. Pixels are written to a MiniLibX image buffer first; the completed image is
   then copied to the window.

### My dominant-axis DDA variation

I independently arrived at a split form of the Digital Differential Analyser
(DDA) line algorithm in `src/line.c`. It is a variation in structure, not a
claim that I invented DDA itself.

For a line from `start` to `end`, the code computes:

```text
dx = abs(end.x - start.x)
dy = abs(end.y - start.y)
```

The larger difference is the **dominant axis**:

- If `dx >= dy`, X changes by exactly `+1` or `-1` each iteration. Y is kept as
  a `double` and changes by `(end.y - start.y) / dx`.
- Otherwise, Y changes by exactly `+1` or `-1`, while X changes by
  `(end.x - start.x) / dy`.
- The non-dominant coordinate is rounded only when the pixel is plotted.
- A one-point line is handled before either division, avoiding division by
  zero.
- The final endpoint is drawn explicitly, so the loop cannot omit it.

This is equivalent in purpose to the common DDA form which takes
`max(abs(dx), abs(dy))` steps, but splitting it into X-dominant and Y-dominant
cases made the direction and loop termination much easier for me to reason
about. The same step index is also used to interpolate the endpoint colours,
so colour changes continuously along an edge.

## Bonus: two kinds of keyboard input

The useful discovery behind the testbed was that “a key press” does not have
to arrive through one universal C mechanism. This program deliberately uses
two different input paths.

### Player 1: X11/MiniLibX events

MiniLibX subscribes the window to X11 key-press and key-release events through
`mlx_hook()`. X11 gives the callback an integer keycode. These events only
belong to a focused graphical window; they are not bytes read from standard
input.

This is also why Player 1 can turn continuously. X11 tells me both "key pressed"
and "key released", so I can set a direction while Left/Right or `Q/E` is held
and reset it on release. Both pairs mean the same thing: rotate the map in
isometric mode, or turn Player 1's camera in FPS. FD 0 only gives Player 2
received bytes; it does not reliably say when a remote key was released. Player
2 therefore turns its own camera in fixed `Q/E` steps.

### Player 2: bytes arriving on file descriptor 0

Player 2 is controlled through `STDIN_FILENO`, which is file descriptor 0.
When FdF is launched after a pipe, FD 0 refers to that pipe rather than the
host keyboard. `nc` can therefore feed bytes received over TCP into FdF without
any socket code in this project.

The render loop must never wait inside `read()`, because a blocking read would
freeze MiniLibX whenever Player 2 was idle. `read_remote_key()` instead performs
a zero-timeout `select()`:

```c
FD_ZERO(&read_fds);
FD_SET(STDIN_FILENO, &read_fds);
select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);
```

`fd_set` is a bitmap-like collection of descriptors. `FD_ZERO` clears it;
`FD_SET` adds FD 0; and `select` reports whether that descriptor can be read
immediately. A zero-valued timeout makes this a poll, so the MiniLibX loop can
continue even when no network input exists. Available bytes are drained and
translated into Player 2 actions. EOF or a read error disables remote input.

This makes use of Unix composition: FdF understands only input bytes, while
`nc` handles listening and transport. The friend does **not** receive an FdF
window. Both graphical views are local windows on the host computer.

## Bonus views and state

The bonus state contains `players[2]` and `cameras[2]`. Using indexed arrays
keeps movement and camera updates shared instead of copying a Player 1 version
and a Player 2 version of each function.

The program initially opens one isometric window. Player 1 starts at the
top-left map point and Player 2 at the opposite, bottom-right point. Pressing
`V` once changes the original window to Player 1's first-person view and creates
a second local window for Player 2. Pressing `V` again destroys the second
viewport and returns the original window to the isometric view.

### Zooming without making the map run away

The isometric bonus keeps a `zoom` multiplier and a screen-space `shift`. The
mandatory projection still calculates the best initial scale/offset to fit and
centre the whole map. The bonus modifies that result rather than writing a
second projection system.

At first, I thought zoom sounds almost too trivial: just multiply `projection.scale`,
right? Almost. A projected point reaches the screen through:

```text
screen = projected_coordinate * scale + offset
```

If I multiply only `scale`, every point expands away from coordinate zero, not
away from the middle of my window. The result is technically zoomed, but it can
also slide toward one side—as if the map is running away from me. :D

To keep the same visual centre fixed, I also move the fitted offset around the
window centre:

```text
new_offset = window_centre
	+ (old_offset - window_centre) * zoom
	+ shift
new_scale = old_scale * zoom
```

`old_offset - window_centre` is the offset measured **from the centre**, not
from screen coordinate zero. Scaling that distance and adding the centre back
means the map grows/shrinks around the middle of the window. Only after that do
I add `shift.x` or `shift.y` for translation.

Translation itself really is the easy part: it adds/subtracts pixels from the
final offsets, moving the entire projection left/right/up/down without changing
the map coordinates. Zoom uses multiplication/division by `1.1` instead of a
fixed addition, so each step feels proportional at both small and large scales.

### How the first-person view actually works

My first thought was:

Is FPS much different from isometric projection, considering I already have my
graphics structs and functions? Or do I need some big 3D library for this?

Then Darren also mentioned quaternions. Apparently those are important for
avoiding gimbal lock in unrestricted 3D rotation.

However, nope, I won't need them here. This is a wireframe height map and the
camera only turns left/right (`yaw`). For this minimum version, ordinary
trigonometry is enough.

Instead of the global world view, I look from where the camera is by subtracting
the camera's coordinates. Then, more maths.

A normal `t_point` answers, "Where is this point in the map's world?" A
`t_camera_point` instead answers three questions from the **player's eyes**:

```c
typedef struct s_camera_point
{
	double	right;
	double	vertical;
	double	depth;
	int		colour;
}			t_camera_point;
```

- `right`: how far the point is to my right (negative means left);
- `vertical`: how far it is above my eye (negative means below); and
- `depth`: how far it is in front of me (negative means behind).

That is the basic trick. FPS rendering does not ask only for a point's absolute
`(x, y, z)`. Instead I ask, "from where I am
standing and facing, is it right/left, up/down, and how far forward?"

#### 1. Put the camera at `(0, 0)`

`camera_point()` first subtracts the camera position:

```c
dx = point.x - camera->x;
dy = point.y - camera->y;
```

For example, if the camera is at `(3, 2)` and the map point is `(8, 6)`, the
point is `(5, 4)` relative to the camera. The world was not moved in memory; I
only changed the coordinate system used for this calculation.

#### 2. Measure against the directions I face

For a yaw angle, the camera has two imaginary direction rulers:

```text
forward = ( cos(yaw), sin(yaw))
right   = (-sin(yaw), cos(yaw))
```

The right ruler is the forward ruler turned 90 degrees. Dot products measure
how much of `(dx, dy)` lies along each ruler:

```c
result.right = -sin(camera->yaw) * dx + cos(camera->yaw) * dy;
result.depth = cos(camera->yaw) * dx + sin(camera->yaw) * dy;
result.vertical = point.z - camera->z;
```

This is "just maths", but specifically it is a change from world coordinates
to camera coordinates. If `yaw == 0`, sine is 0 and cosine is 1, so the camera
looks along world +X: `depth = dx` and `right = dy`. When yaw changes, the map
points stay still but the two camera rulers rotate. That is why turning works.

Vertical is simpler because this camera has no pitch: world Z is still camera
up. Each camera follows its player's `(x, y)` map point and sits at
`terrain_z + CAMERA_EYE_HEIGHT`.

#### 3. Perspective comes from dividing by depth

`draw_perspective_edge()` converts camera coordinates to screen coordinates:

```text
screen_x = centre_x + focal * right / depth
screen_y = centre_y - focal * vertical / depth
```

**This division is what makes it look first-person**. Consider two points equally
far to the right:

```text
right = 2, depth = 2   -> right / depth = 1.0 -> far from screen centre
right = 2, depth = 20  -> right / depth = 0.1 -> close to screen centre
```

The distant point looks smaller/closer to the middle. The same happens to
height through `vertical / depth`. Screen Y uses subtraction because world Z
increases upward, while image Y increases downward.

`focal` converts that ratio to pixels. It is calculated from the window width
and a 60-degree field of view:

```text
focal = window_width / (2 * tan(field_of_view / 2))
```

A smaller field of view produces a larger focal length (zoomed in); a larger
field of view produces a wider view.

#### 4. Do not project what is behind my head

Perspective divides by `depth`, so zero would divide by zero and a negative
depth would mirror points behind the camera. The renderer rejects an edge if
either endpoint is at or behind `CAMERA_NEAR` (`0.1`). This is intentionally
simple near-plane rejection, not full clipping: if an edge crosses the plane,
its visible part is also discarded. That can make very close edges pop out.

#### 5. Reuse the mandatory renderer

Once an edge's two 3D endpoints become two screen `(x, y)` points,
`draw_perspective_edge()` hands them to my existing dominant-axis
`draw_line()`.

`draw_neighbours()` again connects only the right and bottom
neighbours, so every grid edge is drawn once. I did not need a second line
algorithm just because the projection changed. Nice :D

`draw_other_player()` uses the same pipeline. It finds the other player's map
point, copies it as `player_top`, adds `PLAYER_SIZE` to the top's Z, and projects
the vertical edge between them. It looks taller when close because
`vertical / depth` becomes larger. `1 - view->player` is a tidy shortcut for
choosing the other player, but only while there are exactly two players.

Finally, `perspective_render()` performs one complete view:

1. move that camera to that player's terrain position;
2. select that player's camera, image and window;
3. clear the whole image buffer, including its row padding;
4. project/draw all right and bottom map edges;
5. draw the other player's vertical marker; and
6. put the completed image into the selected MiniLibX window.

`game_render()` calls this same function once for Player 1 and once for Player
2. Both views share the loaded map and MLX connection, but each window needs its
own image buffer. There is no Z-buffer or hidden-line removal, so later pixels
simply overwrite earlier ones. Closing either window goes through one cleanup
path which destroys the second image/window first, then the shared map, main
image, main window, and MiniLibX display.

## The `.bonus` marker Makefile trick

Mandatory and bonus must both create an executable named `fdf`. That causes a
small Make problem: after I build one version, the file `fdf` already exists.
Make normally compares timestamps and may decide there is nothing to do, even
when I ask for the other version.

I could solve that with `make re` every time, but recompiling every `.c` file
and both libraries just to switch the final executable is rather wasteful. The
Makefile instead uses an empty `.bonus` file as a note to itself:

```make
BONUS_MARKER := .bonus

bonus: $(BONUS_MARKER)
```

The filename starts with `.`, so it is hidden in a normal `ls`. It contains no
configuration or program data. Its existence simply means:

```text
`.bonus` exists     -> the current `fdf` was linked from bonus objects
`.bonus` is absent  -> the current `fdf` is mandatory (or not built yet)
```

### Going from mandatory to bonus

```make
$(BONUS_MARKER): $(BONUS_OBJS) $(GNL_OBJS) $(LIBFT) $(MLX)
	$(CC) $(CFLAGS) $(BONUS_OBJS) $(GNL_OBJS) $(LIBFT) \
		$(MLX_FLAGS) -o $(NAME)
	touch $(BONUS_MARKER)
```

When I run `make bonus` and `.bonus` does not exist, its target is missing, so
Make links `fdf` using `BONUS_OBJS`. After linking succeeds, `touch .bonus`
creates an empty marker (or only updates its timestamp). `touch` does **not**
change the executable.

Because the marker depends on all bonus objects and libraries, changing bonus
source also makes the marker older than a prerequisite. Make then relinks the
bonus and touches the marker again.

### Going from bonus back to mandatory

This conditional is evaluated when Make reads the Makefile:

```make
ifneq ($(wildcard $(BONUS_MARKER)),)
$(NAME): FORCE
endif

FORCE:
```

Here is what each unfamiliar part means:

- `$(wildcard .bonus)` asks Make to expand an existing matching pathname. It
  becomes `.bonus` when the file exists, or an empty string when it does not.
- `ifneq (value,)` means "if `value` is not empty". Therefore, this block is
  enabled only when the last build was bonus.
- `$(NAME): FORCE` adds `FORCE` as another prerequisite of `fdf`. Make permits
  a target's prerequisites to be declared across multiple rules.
- `FORCE` has no commands and is listed under `.PHONY`. A phony target does not
  represent a real file, so Make treats it as needing an update every time.

Therefore, if `.bonus` exists and I run plain `make`, `FORCE` makes the `fdf`
link recipe run even if `fdf` is newer than every mandatory object:

```make
$(NAME): $(OBJS) $(GNL_OBJS) $(LIBFT) $(MLX)
	$(CC) $(CFLAGS) $(OBJS) $(GNL_OBJS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)
	rm -f $(BONUS_MARKER)
```

That recipe replaces `fdf` using only the mandatory `OBJS`, then removes the
marker. On the next separate `make` invocation, `wildcard` finds no `.bonus`,
so `fdf` no longer receives the `FORCE` prerequisite and normal timestamp
checking resumes.

`FORCE` does not delete object files, add bonus code to mandatory, or bloat the
binary. The linker command's explicit object list decides what goes inside
`fdf`; `FORCE` only says, "run that recipe this time." `clean` also removes the
marker because a marker describing a cleaned/generated executable would be
stale and misleading.

In short:

```text
make bonus : link BONUS_OBJS -> fdf -> touch .bonus
make       : if .bonus exists, force-link OBJS -> fdf -> remove .bonus
make again : ordinary timestamps; nothing relinks unnecessarily
```

## Instructions

This checkout supports the Linux/X11 MiniLibX used at school and the included
macOS MiniLibX. The remote-input demonstration below is intended for Linux,
where `nc` and `hostname -I` are normally available.

### Mandatory evaluation

```sh
make fclean
make
./fdf test_maps/42.fdf
```

Press `ESC` or close the window to exit.

### Bonus evaluation without networking

```sh
make fclean
make bonus
./fdf test_maps/42.fdf
```

This still demonstrates rotation, Player 1 movement, `V`, and both local
camera windows. FD 0 remains the terminal in this form, so typed Player 2 input
normally needs Enter because the terminal is in canonical mode.

### Bonus evaluation with a second terminal/computer

Both computers must be reachable on the same network and the chosen port must
not be blocked. On the **host computer**, first find its LAN address:

```sh
hostname -I
```

Then build and start the listener/FdF pipeline. FdF can start before the other
computer
connects because its FD 0 polling is non-blocking:

```sh
make bonus
nc -l 3333 | ./fdf test_maps/42.fdf
```

On **the other computer**, replace `HOST_IP` with the host address:

```sh
nc HOST_IP 3333
```

In an ordinary terminal, you normally need to press Enter before the commands are parsed. For
single-key input *without* Enter, you can **temporarily disable canonical
input** and restore the terminal afterward:

```sh
old_tty=$(stty -g)
trap 'stty "$old_tty"' EXIT INT TERM
stty -icanon -echo
nc HOST_IP 3333
stty "$old_tty"
trap - EXIT INT TERM
```

If the connection succeeds, `nc` may print nothing. The visible confirmation
is Player 2 moving in the host's FdF window.

Also different `nc` implementations have slightly different listen syntax. On a
version which rejects the host command above, try `nc -l -p 3333`.

### Controls

| Input source | Isometric mode | First-person mode |
|---|---|---|
| Host `W A S D` | Move Player 1 | Move Player 1 |
| Host `Left / Right` | Rotate the map | Turn Player 1 camera |
| Host `Q / E` | Rotate the map | Turn Player 1 camera |
| Host `V` | Open the two first-person views | Return to one isometric view |
| Host `-` / `=` | Zoom out/in | No action |
| Host `Z` / `C` | Translate view left/right | No action |
| Host `R` / `F` | Translate view up/down | No action |
| Numpad `4` / `6` | Translate view left/right | No action |
| Numpad `8` / `5` | Translate view up/down | No action |
| Host `ESC` / window close | Exit and clean both views | Exit and clean both views |
| Remote `W A S D` | Move Player 2 | Move Player 2 |
| Remote `Q / E` | Turn Player 2 camera | Turn Player 2 camera |

Movement is grid-based and cannot leave the map. This testbed has no collision,
authentication, reconnect protocol, interpolation, physics, or win condition.
The `=` key is also the physical `+` key on the usual keyboard layout, so either
plain `=` or shifted `+` selects the zoom-in key.

## Resources

- The FdF subject supplied by 42
- The Norm v4.1 supplied by 42
- MiniLibX manual pages and the Linux/macOS MiniLibX source included here
- The system manual pages for `read(2)`, `select(2)`, `gettimeofday(2)`, and
  `stty(1)`
- Darren's showcase repository, consulted for project structure and image
  buffer usage; no source code was copied
- Peer discussion, including the suggestion that a first-person projection
  could be derived from the existing projection work
- My personalised YouTube/Instagram recommendations are mostly nerdy programming
  content, so some inspiration also came from seeing other CS concepts/videos

### Use of AI

Although AI was used substantially, it is disclosed here rather than presented
as fully independent work.

It was mainly used to:

- **generate the initial scaffold/documentation,
clarify subject and Norm constraints, explain MiniLibX and projection concepts**,
- review code and refactor shared mandatory/bonus rendering,
- help implement rotation by adapting the existing projection path,
- help me learn `select()` and implement FD 0 input alongside MLX hooks,
- explain and help implement the first-person projection maths/algorithm, and
- help add a second window and image while retaining one shared MLX connection,
  including cleanup and `NULL` handling.

I directed the design and tested the results. My manual work also included
mechanical changes I considered straightforward—for example converting a
single player into indexed player storage, extending capacity/state, adapting
call sites, and copying or moving small pieces while refactoring.

AI was also used as an interactive tutor while I questioned choices, modified
or rolled back changes instead of blindly retaining them, and examined
callbacks, cleanup ownership, `select`, duplicated functions, camera yaw, and
Makefile bonus switching.

Which means the bonus should not be judged from the code alone as proof that I
understood every generated line immediately. If needed, I should be able to
derive the projection equations, trace ownership and cleanup,
explain X11 events versus FD input, and modify the implementation without AI.

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
isometric projection, two movable points, two first-person cameras, and a way
for a second terminal (including one on another computer) to provide controls.
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
a second local window for Player 2. It does not currently toggle back.

The first-person renderer translates points relative to a camera, rotates them
by camera yaw, rejects edges behind the near plane, and applies perspective:

```text
screen_x = centre_x + focal * right / depth
screen_y = centre_y - focal * vertical / depth
```

Only yaw is required for this minimum version, so quaternions would add
complexity without solving a present problem. Each camera sits at its player's
map height plus an eye-height offset. Closing either window goes through one
cleanup path which destroys the second image/window first, then the shared map,
main image, main window, and MiniLibX display.

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

Then build and start the listener/FdF pipeline. FdF can start before the friend
connects because its FD 0 polling is non-blocking:

```sh
make bonus
nc -l 3333 | ./fdf test_maps/42.fdf
```

On the **friend's computer**, replace `HOST_IP` with the host address:

```sh
nc HOST_IP 3333
```

In an ordinary terminal, the friend types controls and presses Enter. For
single-key input without Enter, the friend can temporarily disable canonical
input and restore the terminal afterward:

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

Different `nc` implementations have slightly different listen syntax. On a
version which rejects the host command above, try `nc -l -p 3333`.

### Controls

| Input source | Isometric mode | First-person mode |
|---|---|---|
| Host `W A S D` | Move Player 1 | Move Player 1 |
| Host `Left / Right` | Rotate the map | Turn Player 1 camera |
| Host `V` | Open the two first-person views | No further change |
| Host `ESC` / window close | Exit and clean both views | Exit and clean both views |
| Remote `W A S D` | Move Player 2 | Move Player 2 |
| Remote `Q / E` | Turn Player 2 camera | Turn Player 2 camera |

Movement is grid-based and cannot leave the map. This testbed has no collision,
authentication, reconnect protocol, interpolation, physics, or win condition.

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

### Use of AI

AI was used substantially and is disclosed here rather than presented as fully
independent work. It was used to generate the initial scaffold/documentation,
clarify subject and Norm constraints, explain MiniLibX and projection concepts,
review code, refactor shared mandatory/bonus rendering, and help implement the
rotation, FD 0 input, first-person projection, and two-window bonus.

I directed the design and tested the results. My manual work also included
mechanical changes I considered straightforward—for example converting a
single player into indexed player storage, extending capacity/state, adapting
call sites, and copying or moving small pieces while refactoring. AI was also
used as an interactive tutor while I questioned choices such as callbacks,
cleanup ownership, `select`, duplicated functions, camera yaw, and Makefile
bonus switching.

Accordingly, the bonus should not be judged from the code alone as proof that I
understood every generated line immediately. Before evaluation I am responsible
for being able to derive the projection equations, trace ownership and cleanup,
explain X11 events versus FD input, and modify the implementation without AI.

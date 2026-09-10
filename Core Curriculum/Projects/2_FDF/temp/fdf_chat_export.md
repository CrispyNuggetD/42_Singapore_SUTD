# FdF development chat — readable export

Exported: 10 September 2026

> This is a reconstructed readable record made from the conversation context
> available to Codex. It is not the application's raw/verbatim chat export.
> User questions are lightly cleaned and earlier assistant answers are
> summarized where their exact wording was unavailable.

## Git cleanup and rotation

**User:** Remove the `mlx_loop.1` edit and the MLX change from the temporary
backup. Keep only the intended changes.

**Outcome:** Unrelated MiniLibX/manual-page changes were separated from the FdF
work.

**User:** Explain all rotation changes. Is `update_rotation_bounds` just
`update_bounds`? Duplication makes evaluation confusing; bonus should extend
mandatory. Improve mandatory and de-static things if needed.

**Outcome:** Mandatory and bonus now share projection, bounds fitting, parsing,
image, line, and cleanup code. Rotation supplies an angle to the common model
projection instead of maintaining copied functions. The centred model X/Y
coordinates rotate around Z before the common isometric transform.

**User:** Why does the close hook accept `void *`, cast to `t_rotation *`, and
call `fdf_destroy`? Why not pass `fdf_destroy` directly?

**Explanation:** MiniLibX calls hooks through a generic callback convention and
passes back the registered `void *` state. A wrapper adapts that convention to
the real structure, performs the whole required cleanup, exits, and returns the
callback's expected integer. `fdf_destroy` alone has the wrong lifecycle and
callback role.

**User:** Why is `mlx_destroy_display` protected by `#ifndef __APPLE__`?

**Explanation:** Linux MiniLibX exposes that function; the included macOS MLX
does not. The platform wrapper calls and frees the display only on Linux.

## Makefile questions

**User:** Explain the `.bonus` marker, `FORCE`, `FORCE` versus `re`, whether
forced mandatory linking bloats the binary, and why cleaning removes `.bonus`.

**Explanation:** Mandatory and bonus produce the same `fdf` pathname. `.bonus`
records which variant currently occupies it. If the marker exists, `FORCE`
makes `make` relink the mandatory object list even when timestamps look current.
It neither cleans nor adds objects. `re` does a broader full clean/rebuild.
Cleaning removes `.bonus` because it describes generated executable state that
no longer exists.

## Remote-input multiplayer testbed

**User:** Can `nc 3333` become a second bonus? Put players at opposite map
corners. Is first-person projection close to what already exists? Maybe `V`
changes view. Do I need a server?

**Explanation:** This was scoped as a multiplayer **testbed**, not a real game.
FdF contains no sockets or game server. A listening `nc` process transports TCP
bytes and pipes them to FdF's standard input. FdF only knows that control bytes
arrived on file descriptor 0.

**User:** Explain `fd_set read_fds`, `FD_ZERO`, and `FD_SET`. Why is this
different from the deleted game implementation? Can input avoid waiting for
Enter?

**Explanation:** `fd_set` is a bitmap-like descriptor collection for `select`.
`FD_ZERO` clears it and `FD_SET(STDIN_FILENO, ...)` adds FD 0. A zero-timeout
`select(STDIN_FILENO + 1, ...)` polls whether input is readable now. This avoids
a blocking `read` freezing the MiniLibX event loop. Terminal canonical mode,
not FdF, is what normally holds typed characters until Enter.

Host evaluation commands:

```sh
hostname -I
make bonus
nc -l 3333 | ./fdf test_maps/42.fdf
```

Friend's command:

```sh
nc HOST_IP 3333
```

`nc` may print nothing after connecting; Player 2 moving in the host window is
the confirmation. For immediate keys on the friend's terminal:

```sh
old_tty=$(stty -g)
trap 'stty "$old_tty"' EXIT INT TERM
stty -icanon -echo
nc HOST_IP 3333
stty "$old_tty"
trap - EXIT INT TERM
```

## Two players and cameras

**User:** Let host WASD control Player 1 through X11, connection input control
Player 2 at the opposite side, and arrows rotate. What is minimally needed for
`V` to enter first-person? Do we need quaternions? Later, give both players
separate windows, remote `Q/E` yaw, and clean everything on error.

**Outcome:** State became `players[2]` and `cameras[2]`. Player 1 begins at
`(0, 0)` and Player 2 at `(width - 1, height - 1)`. Before `V`, arrows rotate
the isometric model. Pressing `V` once turns the original window into Player
1's first-person camera and opens a second **local** Player 2 camera window.
Host arrows turn Player 1; remote Q/E turns Player 2. Both views share one MLX
connection and map, but have separate images/windows. Either close event uses
one complete cleanup path. Quaternions are unnecessary for yaw-only rotation.

Relevant commits:

```text
54f6575 refactor(fdf): share renderer with rotation bonus
ca74512 feat(fdf): add remote player and first-person view
38679d5 feat(fdf): add two-player camera windows
eb4fa29 docs(fdf): document renderer and bonus testbed
```

## Dominant-axis DDA

The line implementation independently arrived at a split DDA structure. It is
a variation in organization, not a claim to have invented DDA.

```text
dx = abs(end.x - start.x)
dy = abs(end.y - start.y)
```

When X dominates, X advances by exactly +1/-1 while floating-point Y advances
by `delta_y / dx`. When Y dominates, the roles reverse. The non-dominant value
is rounded only while plotting. A one-point case avoids division by zero and
the final endpoint is explicit. The same step counter interpolates colours.

## Perspective renderer

**User:** Explain all of `perspective_bonus.c`, especially:

```c
result.right = -sin(camera->yaw) * dx + cos(camera->yaw) * dy;
```

The whole transformation is:

```text
world point
 -> subtract camera position
 -> resolve displacement into camera right/depth coordinates
 -> reject geometry at/behind the near plane
 -> divide right and vertical by depth
 -> obtain screen X/Y
 -> reuse mandatory DDA for the edge
```

Camera direction vectors are:

```text
forward = ( cos(yaw), sin(yaw))
right   = (-sin(yaw), cos(yaw))
```

Dotting `(dx, dy)` with `right` gives horizontal camera-space position; dotting
it with `forward` gives depth. Vertical is `point.z - camera.z` because this
minimum camera has no pitch/roll.

Perspective projection is:

```text
screen_x = centre_x + focal * right / depth
screen_y = centre_y - focal * vertical / depth
```

Division by depth makes distant objects smaller. Image Y grows downward, hence
the vertical subtraction. `draw_neighbours` draws only right and bottom map
edges, exactly once. `draw_other_player` uses `1 - current_player` (valid only
for exactly two players) and represents the opponent as a vertical coloured
edge. `perspective_render` follows the camera, clears the padded image buffer,
draws terrain/opponent, and puts the image into the chosen window.

Known simplifications: no pitch, roll, quaternion, Z-buffer, hidden-line
removal, screen clipping, filled triangles, or proper near-plane clipping. If
one endpoint fails the near-plane test, the current renderer discards the whole
edge instead of clipping its visible part.

## README and AI disclosure

**User:** Make a README meeting 42 requirements. Explain the self-derived DDA
variation, X11 events versus FD 0, execution/evaluation commands, manual work,
and honest AI use. Tell me where understanding seemed weak and match my voice.

**Outcome:** The README now contains the required opening statement,
description, instructions, resources, architecture, controls, limitations, and
substantial AI disclosure. Codex used this conversation and older local READMEs
for tone but could not access private ChatGPT account history or custom
instructions.

Topics to rehearse before evaluation:

1. Derive camera right/depth using dot products.
2. Explain perspective division by depth.
3. Distinguish near-plane rejection from real clipping.
4. Explain `fd_set`, the macros, and why `nfds` is FD 0 + 1.
5. Trace cleanup after partly creating the second view.
6. Explain shared MLX/map ownership versus per-window image ownership.

The final README was committed and pushed as `eb4fa29`.

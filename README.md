> **Forgot to push before leaving for the weekend? [Get private backups on screen lock + zsh/tmux helpers](Shell%20automations/README.md).** Helpers-only download available. Requires setup; you still need to share changes with teammates.

# 42 Repository for Christopher Nah (hnah)

Hi, I’m **Christopher Nah** (**hnah** on Intra), a student at **42 Singapore**. Questions, ideas, or just saying hi? Find me on the **global 42 Slack**, or [here on GitHub](https://github.com/CrispyNuggetD).

Want a dock icon that commits, pushes and locks your screen?
[Go Break quick start](Shell%20automations/gobreak/README.md#quick-start)
installs just that feature, with optional Codex commit bodies.

Optional apps: `setupapps` chooses a personal list for `dailylogin` (off by
default); `desktopapp` creates a launcher and offers dock pinning.
[App setup and storage guide](Shell%20automations/apps/README.md).

## Terminal setup after cloning

With Python 3 and zsh installed, open a **zsh** terminal and run these commands
from inside your cloned repository folder:

```sh
python3 sync_zshrc.py
source ~/.zshrc
setupzshrc
```

`sync_zshrc.py` installs the shared terminal helpers and installs or refreshes
the [Go Break icon](Shell%20automations/gobreak/README.md), preserving any
standalone repo selection. `setupzshrc` opens the
setup wizard: enter your own email, repository paths, and current project, then
choose which tasks to enable and save. Optional services and automatic updates
default to off; an existing LAN42 install is offered enabled, preserving saved choices. The installer also enables mouse scrolling in `~/.tmux.conf`.
For a tmux session that is already running, run `tmux set -g mouse on` once.

Running `python3 setup_zshrc.py` alone only saves settings; it does not install
the terminal helpers. Run `setupzshrc` again whenever you want to change them.

Install `tmux` as well to use the workspace (`sudo apt install tmux` on Ubuntu,
`brew install tmux` on Mac). This is part of the public workflow for every user.

When ready, run `dli` to start the tasks you enabled and then your own login script.
`dailylogin` runs only the shared tasks; `lgi` runs only your personal script. See the
[full shell setup guide](Study%20Notes/Codex/shell-setup.md) for more details.

The workspace uses two desktop terminal windows when LAN42 is enabled:
Work contains Codex, mailbox, **Current Project**, a retained **dailylogin** log,
and your personal login script; LAN42 contains its HQ, games and tools in its own
session. Enabled tasks determine which tabs appear. An already running managed
tab is reused. `dli` attaches Work in the invoking terminal; existing shells cannot
be moved into tmux, but their dailylogin output is saved locally for the log tab.
Use `workterm` and `lanterm` to reconnect. `dailylogin` does not run your login script.

Both terminals default to maximized, retaining the title bar and desktop controls.
Set `DAILY_TMUX_MAXIMIZE=FALSE` in your personal settings to disable that, or use
`setupzshrc`. GNOME Terminal and xterm are supported on Linux; macOS uses Terminal.
Set `LAN42_TMUX_SESSION` to choose the separate LAN42 session name (default `lan42`).

`cdcurproj` changes directory only; `curproj` also cleans and opens VS Code.
To share a project choice between your own workstations, put only an assignment
such as `export current_proj=3_Philosophers` in a file in your **private** repository.
On each workstation set `ZSHRC_PORTABLE_SETTINGS_FILE` to that file's local path
in `~/.42-shell-settings.zsh`. `refreshproject`, `cdcurproj`, `curproj`, and dailylogin
reload it; enable repository pulls to receive committed changes on login.
Each student's profile is optional and independent. Keep machine paths, SSH material,
API credentials and authentication state local; never copy a whole shell profile
or home configuration into the public repository.

### Your own final login step

The `loginitems` flag defaults to `TRUE`. Setup creates `~/.42-login-items.sh`
with instructions and commented examples, and no active commands. Add your own
Bash commands there: `dli` starts them **last** in a retained `login-items` tmux tab,
even if a shared daily task fails. Set `LOGIN_ITEMS_TMUX_ROLE` in personal settings
to give that tab another name. A `clear` in your script affects only its own pane.
The pane retains script failures and output; `dli` reports launch status, while the
script's eventual exit status appears in its pane.
If the script is missing, the enabled step creates it before running it.
Existing scripts are never overwritten by setup or updates. Each user gets their
own file; no personal solver or project commands are shipped in this template.

Edit `~/.42-shell-settings.zsh` to set `loginitems=FALSE` to disable this step,
or set `LOGIN_ITEMS_SCRIPT` to another path, then run `source ~/.zshrc`.
`setupzshrc` also offers these settings. Arguments to `dli` are passed to your
script. It runs in Bash as a separate process, so zsh aliases/functions are not
available and `cd` does not change the parent terminal's directory. Manual
`dli` and `lgi` calls repeat your commands; make them safe to run more than once.

### Private backups when you lock the screen

After installing the shell helpers, run:

```sh
setupprivatebackup
```

Create your own **empty private** GitHub repository (for example `42-school-wip`),
then paste its SSH URL when asked: `git@github.com:YOUR_USER/42-school-wip.git`.
The installer checks SSH access, rejects publicly visible repositories, and asks
you to confirm the private setting. SSH authentication must already work without
interactive prompts. This uses a Linux desktop with systemd, Python 3 and PyGObject
(`gi`, normally provided by GNOME). It does not change system lock/logout settings.

`privatebackup` defaults to `TRUE`. Until setup is complete, **every `dli` prints
an instruction to run `setupprivatebackup`**. Setup does not happen automatically.
The setup wizard also offers this flag. Set `privatebackup=FALSE` in
`~/.42-shell-settings.zsh` to stop automatic backups and the reminder; reload
`.zshrc` to silence the reminder. The watcher reads the saved setting on each lock.

Once configured, locking or switching away from your desktop saves all tracked
and non-ignored new files in your selected school repo, including deletions, and
pushes a snapshot to `wip/42-school` in your private repo. This is the equivalent
file selection to `git add --all`; known credential filenames stop the checkpoint
for local review. Existing ignore rules still apply to new files; tracked files
remain included. This is not a complete secret detector. Nested repositories and
submodule contents are not traversed. Unsaved editor buffers are not captured.

The installer adds a `backup` remote. Snapshots have their own local Git history,
so your public branch, current HEAD, staged changes and public `origin` stay intact.
No-change locks retry the existing push instead of creating duplicate commits.
Uploads use non-interactive authentication and never force-push. A failed upload
keeps its local checkpoint. Keep the destination private after setup.

Use `privatebackupnow` for a manual checkpoint, `privatebackupnow --preview` to
list included files, and `privatebackupstatus` to check setup/service status.
Configuration is in `~/.config/42-lock-checkpoint.json`; logs are in
`~/.local/state/42-lock-backup/watch.log` (XDG overrides apply).
The user service starts at desktop login. Codam activation signals and GNOME lock
signals trigger it; switching to another TTY also counts as leaving the desktop.
Tmux, Codex and desktop applications keep running. Campus forced logout can still
terminate them later. The full `leaveschool` command separately closes tmux after
successful syncing and is not used by this lock hook.

School backups contain your own school files. Friends' setups do not export Codex
chats by default. The owner's optional chat workflow copies session JSONL files
only into the separate private workspace's `42/codex_cache/`, then snapshots that
folder to its private `wip/42-chat-backup` branch. It never puts chats in the school
WIP repo or public school repo.

For fun, run `badapple` to play Bad Apple in your terminal. On first use it
sets up `~/joke/play.sh` with assets from
[bad-apple-ascii](https://github.com/trung-kieen/bad-apple-ascii).
No sudo needed; audio uses VLC if available. Use `badapple --silent` for no
audio, and Ctrl+C to stop.

PISCINE 7:
21 JUL - 15 AUG 2025

INTAKE 4:
17 NOV 2025 - 18 NOV 2028 (Irreversible Information Loss)

Common Core ETA:
16 NOV 2026
(At least that is the goal I’m optimistically trying to hit, with expected headwinds…)

# My 42 Plan

I was stressing out during first day orientation as everyone wanted to compete for Core graduation speed leaderboard.

But I realised while that’s fun and challenging (being competitive), it doesn’t and shouldn’t apply to me because I’m concurrently juggling therapy and trying to maintain inner balance anyway. (Last thing I want is a relapse, leading to a Freeze!).

So I opted for the 12 months track that recommends, 35-45 hours a week, which I will have to study on weekends too. However, for my challenges, I probably have an (at least) “1.5x Multiplier Bonus Effect”, needing 60+ hours to get the same amount of work done.

# 42 Tips for others if you chance upon my repo.

Get an iPad if you can, with a keyboard. It's like having a Macbook/ MacOS but you can poke the screen too (and more)!

Then set up working environment by downloading “Working Copy” IDE/ Application for Git Versioning. Costs $40 (SGD, 2025) to do Git Push but it’s a necessary evil I guess. But hey, it’s a good app and Anders has to eat too! And it’s a good study investment.

Having an Apple Pencil is a neat bonus (Direct annotation + Note-taking/ Apple’s “Freeform” App for creative brainstorming canvas). But otherwise keep a Personal Notebook or just have Pen + Paper with you always like I also do (Batteries also die. And sometimes iPad just doesn’t apply). 

That being said, it’s always easier to sketch out ideas to think of a solution to the problem. Use whiteboards if needed when discussing with peers. Or just scribble and talk with yourself at the whiteboards. (That’s probably okay too, I guess).

# Weekly Progress 

(Detailed Daily Tracker in “Daily Progress Tracker.md”)

Week 1: (Stalled progress)
- Settling some Family stuff + Sick
- Set up working environment (iPad + Keyboard + “Working Copy” IDE/ Application for Git Versioning).
- ChatGPT “refined” Post-piscine custom project instructions for “Computer Science” folder/ project
- Admin (SUTD email, slack, etc…)
- Careful “re-solidifying” of C fundamentals/ Piscine content.
- Git Clone libft (At least… Just take one step at a time!)

Week 2: (Starting up motor)
- Added zsh setup guide for curproj (cd to cur. proj. + Check git status), 
- Also zsh setup guide for syncproj (copy cur. proj to public GitHub - THIS - and git push), 
- Synced folder libft.

1 Month:
- Finished Libft, but I'm Pace 22 now. Whoops. But expected due to my challenges.
- Partook in peer discussions/ debates for get\_next\_line, and using indexing table with function points for ft\_printf to not "change logic" during evaluations?
- Started on ft\_printf

# PLAN FOR THIS WEEK

Week: 6 (By End 2025)
- Refresh ft\_split to learn double malloc + double freeing
- Master function pointer void prototypes and typedefs
- Learn finish what entails a good Parser
- Finish looking through subject.pdf of ft\_printf
- Finish Parser (1 of 3) of ft\_printf (Parser --> Dispatcher --> Handler)

`leavesch` acts as your personal shutdown: save/push work, request normal quits
from your desktop apps, save/push any files they flushed on exit, close tmux,
then request terminal closure last and lock.
It does not log out or power off the machine. Quit requests use native tray/GTK
Quit actions or supported normal window-close messages on Linux X11, and normal
AppKit quit requests on Mac. Routine keyboard quit guards are avoided; save,
active-transfer and unknown confirmation dialogs are not blindly accepted.
If an app stays open, tmux is retained and the screen still locks. Terminal
prompts, if any, remain for local review; terminal-stage output is in
`~/.local/state/42-shell/terminal-quit.log` (respects `XDG_STATE_HOME`).

Configure `closeappsonleave=FALSE` to disable app-wide shutdown (the existing
Chrome-only option still applies), or `LEAVESCH_KEEP_APPS=slack,code` to keep
specific app classes/bundle IDs. `closechromeonleave=FALSE` exempts Chrome too.
Linux needs an X11 session and Python GObject support (`python3-gi` on Ubuntu);
Wayland reports that automatic app-wide closing is unavailable rather than
force-killing anything. Go Break and ordinary screen locks keep apps running.

LAN42 separates guest/game hosting from its approved authentication/API service.
Guests keep chat and games; API-backed views require verified 42 Singapore
membership. **Someone must keep an approved host running on a campus workstation
(currently hnah’s).** Installing or pulling LAN42 alone does not provide shared
sign-in or API views. When that host is offline, guest chat/games remain usable,
but shared sign-in and fresh cluster views are unavailable. Stable, always-on
hosting is future work. With the campus host online, SG students can `/signin`,
then `/clusters` or `/clusters 2`
to see current API-reported logins and occupied campus seats—not just an empty
floor plan. The approved host (currently hnah’s) shares a 30-second snapshot across
its clients: ten simultaneous requests share one refresh, which may span multiple
API pages. Redrawing the display does not fetch again.
The clickable LAN42 footer offers F2 sign in/out (HQ), F3 system mute, F4
output 0/30%, F5 uwu once and F6 automatic uwu on/off. Sound consent remains
separate; terminal users may need Fn with the function keys.
On-campus guest HQs now explain availability at startup. With the owner-approved
public trust installed, waiting clients check the pinned host every 120 seconds
by default and show its return, with one desktop arrival alert per computer when
the host channel is enabled. Important host/broadcast/release/offline-warning
alerts default to on; chat/DM/poke popups default to off. `/notify` lists channels,
`/notify dms on` enables DMs, and `/notify all off` silences even important alerts.
Existing explicit popup denials and OS permissions are respected.
The approved campus host also starts a 42 Singapore KoelDrop flock. Eligible
verified campus users choose join once, always join or never show again. Each
join uses a fresh service subfolder; old local copies survive shutdown and are
never automatically deleted. See [the flock guide](https://github.com/CrispyNuggetD/42sg-campus-lan/blob/main/docs/file-sharing.md).
This polling uses LAN42 endpoints, not the 42 API;
it never forces OAuth or plays a sound automatically. Missing trust is explained
instead of silently waiting. The initial admin and API operator is hnah (numeric 42 ID 234449).
Admins, API operators and trusted service machines have independent allowlists.
Only API operators may explicitly cache/export/download Singapore-scoped data.
Read-only Singapore scope is the owner's application-form promise; the upstream
student credential may have broader permissions. Students never receive it.

For hnah’s existing managed school host, LAN42 now migrates the legacy access
policy automatically on its next start after updating. `dailylogin` opens the
launcher first; LAN42 initializes authentication after its Git update, so stale
pre-update checks cannot block the upgrade. No one-time script or manual upgrade
command is needed. Existing policies/revocations are preserved; new hosts still
need explicit approval. The shell installer does not grant admins, copy
credentials or install Tailscale. Offsite local
owner API views require no public forwarding; KoelDrop retains its physical
campus checks. See the [LAN42 access policy and migration instructions](https://github.com/CrispyNuggetD/42sg-campus-lan/blob/main/docs/access-policy.md).

Both shell installer entry points recommend LAN42 when it is missing, including
a suggested path and short feature list sourced from the marked section of
[LAN42's README](https://github.com/CrispyNuggetD/42sg-campus-lan#highlights).
Online installs read the current list; offline installs identify their dated
snapshot. See [the maintenance workflow](Shell%20automations/lan42-integration.md)
for refreshing it. Existing LAN42 installs are detected and get tmux setup guidance.

### Workflow updates and a full re-login

With `autoupdatezshrc=TRUE`, `dailylogin` and `dli` first pull and install the
shared code, then hand control to the freshly sourced entry function. The old
entry returns without executing its task body. A per-invocation guard allows
one pull/handoff only, so the new workflow runs once rather than recursively
pulling. Pull/install/reload failures stop before any login tasks run. Personal
settings and startup scripts stay outside the shared block.

`resetdailylogin` clears the local startup marker and reloads shell settings;
other interactive shells pick it up at their next prompt. `relogin` is a full
terminal-workspace restart on Linux X11 and Mac: it stops old tmux panes/jobs
and this user's managed local LAN42 node, closes terminal windows, clears the
marker, and opens a fresh terminal to run `dli` and create fresh sessions.
Unrelated desktop apps and shared/campus authentication servers are unaffected.

If running work is detected (including inside tmux), it asks `y/N`. **y** agrees
to ending that work and allows force-closing blocked terminal hosts; **n** or
Enter cancels the restart. Idle workspaces skip the question. `relogin --check`
previews without stopping or launching anything. Output is recorded in
`~/.local/state/42-shell/relogin.log` (`XDG_STATE_HOME` overrides this).
The commands are part of the public installer; Linux uses GNOME Terminal/xterm,
and Mac uses Terminal/AppKit. Local startup paths and enabled tasks remain
per-user choices. OS-level services unrelated to this terminal workspace keep
running: this simulates the shell login workflow, without logging out the OS.

### The same daily workflow on Mac

Use the shared installer and your Mac's own `~/.42-shell-settings.zsh`.
`dli` uses tmux on both platforms; the optional LAN42 workspace remains separate.
The Mac's `~/.42-login-items.sh` starts with usage text and no personal commands.
Existing scripts are preserved. Leave `LOGIN_ITEMS_TMUX_ROLE=login-items` on Mac;
Linux-specific tester commands stay in the Linux owner's local script.

The app list stays local in `~/.config/42-shell/apps.json` (or `XDG_CONFIG_HOME`).
For example, `loginapps add 'open -a Music' 'open -a Safari'` opts into those Mac
apps. `setupapps` also accepts an app name or `.app` path on Mac. LaunchServices
reuses running apps. No Linux app list is copied during updates.

Mac brightness uses an installed `brightness` command (0–1 scale), or set
`MAC_BRIGHTNESS_FUNCTION` to a local shell function taking a percentage 0–100.
The existing `adjustbrightness` and `setbrightness` preferences apply to both
platforms. No brightness tool or permissions are installed automatically.

On the Mac processor, enable `startmailbox=TRUE` and set `MAC_REMOTE_ROOT` to its
source clone. The tmux mailbox tab displays the existing launchd service's local
log by default. Closing the tab does not stop the listener. `MAC_MAILBOX_RUNTIME`
can select a nondefault installed runtime. `MAC_MAILBOX_MODE=tmux` runs the
installed processor in the foreground only when its worker lock is free; it
never stops another worker or disables launchd. Switch supervision locally while
idle if you explicitly prefer foreground mode. Identities and approvals are untouched.

`leavesch` on Mac exports continued school chats into the private return handoff,
backs up transcript JSONL files to the separate private repository, syncs work,
requests normal app Quit, saves changes again, and closes terminals last. Save
prompts or sync failures keep the tmux workspace open. It attempts the native Mac
screen lock last. The Quit requests use AppKit; the fallback lock shortcut can
require local macOS automation/accessibility permission. `LEAVESCH_KEEP_APPS`
provides a local comma-separated exception list. Public updates do not replace
these machine-specific preferences or copy credentials between computers.

### API-backed campus login reports

`hourslogged` and `loginbefore12` now prefer your own 42 API location records.
Dailylogin uses these when `loginreports=TRUE`. The rolling window is the last
seven calendar dates in the system timezone (`LOGIN_HOURS_TIMEZONE` can override
it). `FORTYTWO_LOGIN` selects your campus login independently of your OS account;
otherwise the login is taken from `MAIL`, then `USER`.

A successful complete API response is labelled **(42 API Verified)** and shows
when it was fetched. Completed API intervals drive totals, daily averages and
the remaining time to the configured existing 50-hour target. Overlapping
sessions are merged and overnight sessions are split at local midnight.
Active API session start times are verified records, but their elapsed duration
is provisional and excluded from the verified total. A target calculation does
not confirm that Intra has granted an award, and future continuous-login times
are estimates because days expire from the rolling window.

When Linux `last` records differ, the report lists both per-day values and keeps
the API figure for its official-data calculation. Local wtmp can contain stale
open sessions or cover only one computer. A successful empty API response means
zero recorded completed time; it is not silently replaced by local time.

API access reuses this machine's existing protected LAN42 API-host configuration
when available through `LAN42_CLONE`. Alternatively, an existing user-local
`FORTYTWO_ACCESS_TOKEN` can authorize the documented HTTPS location endpoint.
Keep authentication local; neither credentials nor tokens belong in shared shell
settings, this repository, or the Mac mailbox. Friends without API access keep
using explicitly labelled local fallback; no host credentials are distributed.
Mac reports can use the API too, but Linux-format `last` comparison is unavailable
there.

A complete API snapshot is cached privately under `~/.cache/42-shell/` (or
`XDG_CACHE_HOME`) for five minutes, so the two daily reports share one fetch.
It contains only your interval timestamps and fetch metadata. Use
`hourslogged --refresh` or `loginbefore12 --refresh` to fetch again. When the API
cannot be reached or pagination/records are incomplete, expired cached data is
not presented as verified: Linux `last` is the labelled fallback. The API's
publication delay and a start-of-day Intra refresh schedule are **not verified**;
the report distinguishes retrieval time from server-side freshness.

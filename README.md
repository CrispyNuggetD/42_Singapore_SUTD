# 42 Repository for Christopher Nah (hnah)

42 Singapore SUTD.

## Terminal setup after cloning

With Python 3 and zsh installed, open a **zsh** terminal and run these commands
from inside your cloned repository folder:

```sh
python3 sync_zshrc.py
source ~/.zshrc
setupzshrc
```

`sync_zshrc.py` installs the shared terminal helpers. `setupzshrc` opens the
setup wizard: enter your own email, repository paths, and current project, then
choose which tasks to enable and save. Optional services and automatic updates
default to off. The installer also enables mouse scrolling in `~/.tmux.conf`.
For a tmux session that is already running, run `tmux set -g mouse on` once.

Running `python3 setup_zshrc.py` alone only saves settings; it does not install
the terminal helpers. Run `setupzshrc` again whenever you want to change them.

When ready, run `dli` to start the tasks you enabled and then your own login script.
`dailylogin` runs only the shared tasks; `lgi` runs only your personal script. See the
[full shell setup guide](Study%20Notes/Codex/shell-setup.md) for more details.

### Your own final login step

The `loginitems` flag defaults to `TRUE`. Setup creates `~/.42-login-items.sh`
with instructions and commented examples, and no active commands. Add your own
Bash commands there: `dli` runs them **last**, even if a shared daily task fails.
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

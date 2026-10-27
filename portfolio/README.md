# Portfolio maintenance

The public README is the concise project index. `README-ARCHIVE.md` preserves
the former detailed README at the repository root so its relative links still
work. No project directories or helper entry points were moved. No one-time
migration and no dailylogin change is required.

## Sources and generated output

- `config.json`: public owner identity, project catalogue and published current
  project. Relative paths only, validated against existing project directories.
- `progress.json`: selected, owner-authorized 42 API snapshot (hnah only), not
  a campus export. No credentials, contact fields, teammates or raw profile.
- Root `portfolio.py`: refresh the README focus, optionally read owner API data,
  and build the static site. `_site` is ignored and contains only site output.
- `index.html`: authored layout; source placeholders are filled at build time.
  The AI disclosure is extracted from the root README, so update that canonical
  wording rather than duplicating it in the HTML template.
- `docs/how-i-work.md`: candid personal tips and dated current toolkit.
- `assets/brand.css`: LAN42 visual foundation copied 27 October 2026; portfolio
  layout is `portfolio.css`. Reuse the navy/mint identity, not duplicated prose.
- Adaptive cursor is vendored from the toolbox’s MIT-licensed component; keep
  `POINTER-LICENSE`. The rest of this repository retains its existing GPL-3.0.

## Current project: same setting, no shell disruption

Keep `current_proj` and `PROJECTS_ROOT` in your existing personal
`~/.42-shell-settings.zsh` (or `ZSHRC_SETTINGS_FILE`). Keep the shared helper file
name and existing `setupzshrc` wizard; no renaming or replacement is needed.
After changing your local focus, run in this owner clone:

```sh
python3 portfolio.py refresh --from-shell
```

This parses only those two exported assignments; it never sources/executes your
settings or publishes other values. It writes the public relative path to
`config.json` and refreshes the README markers. Review and commit both files.
The build rejects a stale README marker rather than guessing a private setting
on GitHub. Other contributors should edit the public config deliberately, not
publish their own local setting as hnah’s focus. CI does not read personal files.

Every page may link to `current-project.html`: it is generated from the same
config and redirects to that project’s README, with a visible link fallback.
Changing focus changes this stable destination at the next deployment.

## Owner marks and deadline snapshot

Refresh on the already approved owner computer with LAN42 installed:

```sh
python3 portfolio.py refresh-api --lan42 /path/to/42sg-campus-lan
```

Existing local operator approval and Singapore membership checks are required.
Only hnah’s selected cursus/project fields are published. Completed projects
are checked using their own detail records; choose the latest validated team
attempt and keep failed retries from erasing validated work. Publish attempt
counts and mark basis, not team identities or raw attempt records. Never put API keys,
raw profile output, shell settings or student exports in GitHub Actions/secrets
for this site. Review a snapshot before publishing it. No refresh happens during
normal dailylogin and public visitors never query the API. Subject-version maxima live in config.json: Libft is 100/100 for Christopher’s
submitted version, where linked lists were mandatory. That maximum is owner-
provided context, not an API final_mark field. Do not compare all marks against
125 or describe a 100/100 result as incomplete.

The countdown runs
locally from the published deadline, shows last checked time and flags snapshots
older than three days. Freeze/deadline changes need another verified snapshot.
No-JavaScript visitors see the actual deadline date instead of a live countdown.

## Build, test and publish

```sh
python3 -m pip install Markdown==3.7
python3 -m unittest test_portfolio
python3 portfolio.py build
python3 -m http.server 8766 --directory _site
```

`.github/workflows/portfolio-pages.yml` builds on main pushes and manual dispatch.
Enable repository Actions and Settings → Pages → Source: GitHub Actions. The
expected URL is https://crispynuggetd.github.io/42_Singapore_SUTD/ after successful
deployment. A pushed workflow is not proof that Pages is enabled/live.

Keep the README and HTML complementary: README offers concise links and prose;
HTML emphasizes current focus, selected work, honest source-labelled progress,
then tools/tips and contact. Test desktop/mobile, JavaScript on/off, real links,
expanded disclosure, deadline staleness and cursor opt-out. Preserve candid tips.


## Hosted workflow runtime

The portfolio workflow pins Ubuntu 24.04 and explicit action releases checked on
27 October 2026. JavaScript actions use Node 24; that runtime belongs to GitHub’s
action runner, not the portfolio’s browser JavaScript or the campus/Mac OS.
Recheck official releases when updating actions. Keep Python 3.12 explicit and
test an OS upgrade before changing the pinned runner. No insecure Node fallback
or warning-suppression environment variable is used.

The shared visual family is **Campus HQ**. Both portfolio page templates link
the reusable cursor package; its persistent bottom-right dock keeps opt-out and
source access available without scrolling to the footer.


## Event Horizon scene

The MIT toolbox Event Horizon component is vendored with HORIZON-LICENSE.
`horizon.html` receives the verified snapshot dates at build time; the author’s
explicit gender setting is male (the reusable component defaults neutral). No
browser API credentials/calls are involved. The astronaut links to hnah’s Intra
and approaches the recorded horizon with time. Keep the horizon’s hover/focus
info, native disclosure, motion preference and JS-off fallback. Refresh API data
through the existing owner command when a deadline/freeze changes. This is a
playful snapshot visualization, not an official standing/eligibility decision.

The scene leads the homepage with a flip-card countdown, an arrival glide and
gentle scroll-responsive floating. Time still determines the baseline distance;
hover/focus holds the profile target still. Motion opt-out covers these effects.

Optional space-backdrop assets add layered scroll/cursor parallax and occasional
comets around the opaque grey reading column. The fixed Space motion control
shares the scene preference. Keep these decorative canvases behind real links,
and preserve grey no-JS and forced-colour fallbacks.

The flight ruler is calendar-linear from `begin_at` to the recorded deadline;
its NOW marker and baseline astronaut position share that scale. The course log
groups existing snapshot projects into M.0–M.3 and shows the latest API
`marked_at` per complete group. These are repository groups, not authoritative
Intra milestone deadlines. Never derive a black-hole date from a pace number or
confuse milestone deadlines with `blackholed_at`.

`data-dock-astronaut` enables the travelling profile link: it catches at the top
of the viewport on scroll-down and returns to the scene on scroll-back. The
link is reparented, not cloned; preserve focus, circular pointer targeting,
motion preferences and offscreen canvas pause.

`data-astronaut-chatter` enables ten brief Unix-time-selected speech bubbles and
state-based mouse proximity reactions, with immediate departure/return feedback. They follow the travelling link, add
no audio/telemetry and respect shared motion off and system preferences.

## Central toolkit catalogue

`docs/toolkit.md` is the canonical component overview and quick integration
guide, rendered into the homepage at `#toolkit`. Keep effect/footer links pointed
there; detailed source, demos and licences remain in FOSS Toolbox. Two tools:
Adaptive Pointer and Event Horizon, with Space backdrop as the latter’s optional
add-on. Update this section whenever options, filenames or dependencies change.

`portfolio/toolkit.html` supplies the curated toolkit showcase; `docs/toolkit.md`
remains the full setup guide inside a native expandable section. Keep the two
consistent without turning the default catalogue into a long documentation dump.
The build adds content hashes to CSS/JS URLs in every generated page, preventing
new markup from reusing stale browser styles/scripts. No JavaScript is required
for the showcase, links or setup disclosures.

Every build emits `build.json` and a visible footer stamp (Actions build number,
commit link and SGT build time; local builds say Local preview). These identify
website deployments, not LAN42 runtime releases. A same-origin, no-store check
on page load or the Check for updates button compares the published manifest.
No GitHub API, visitor telemetry, polling or credentials are involved. Without
JavaScript the stamp and deployment-history link remain visible.

The astronaut offers GitHub for everyone and Intra for 42 students through a
labelled chooser. Both open in new tabs. Preserve authored fallback links,
HTTPS validation, keyboard/Escape handling and the panel's viewport bounds.

## Preserve the visitor's place

The build enforces new tabs with `noopener noreferrer` for external HTTP(S)
anchors, including rendered Markdown. Relative/fragment links and links across
the same Campus HQ Pages host stay in the current tab. Dynamic links must follow
the same rule; never add an automatic off-site redirect. Preserve native links
for no-JavaScript visitors. Test both internal and external destinations.

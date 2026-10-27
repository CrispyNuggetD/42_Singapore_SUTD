# Portfolio maintenance for agents

Read README.md and portfolio/README.md before changing the public presentation.
Keep current-project links driven by config.json; on the owner clone use
`python3 tools/portfolio.py refresh --from-shell` when checking current_proj. Parse only
the two documented personal settings; never execute/source or publish the file.
Other contributors should use public config, not import their own local focus.

README is concise; README-ARCHIVE.md preserves the full previous README.
Maintain candid tips in docs/how-i-work.md and date personal model/tool choices.
Do not move deep project directories or change dailylogin to redesign the site.
No one-time migration script is needed. Reuse the LAN42 navy/mint styling and
MIT cursor copy with its licence; the main repository remains GPL-3.0.

Own API export requires existing approved hnah operator and SG membership.
Publish only selected own grades/cursus dates, never credentials/raw profiles or
teammate data. Check retries; a failed retry must not erase validated work.
Denominators depend on subject version: Libft is max 100 for the owner’s submitted
version, not an incomplete 100/125. API reports marks; maxima are separate owner-
provided context in config.json. Keep source/date/staleness honest. Public clients
never fetch the API. Test with `PYTHONPATH=tools python3 -m unittest discover -s tests -p test_portfolio.py`, build and check
mobile/desktop, JavaScript on/off, links and deadline/cursor fallbacks.

Event Horizon is a separate MIT-licensed toolbox component with a canvas backdrop
and semantic profile link/horizon disclosure. Preserve its motion opt-out,
offscreen/hidden-tab pause, no-JS content and explicit gender configuration.
Never infer gender from API/profile names. The author selects male; reusable
default is neutral. Use only the existing owner snapshot for deadline data.

Campus HQ link policy: same-site links (including the shared Pages host) stay
in the same tab. External HTTP(S) links open a new tab with noopener/noreferrer;
apply this to generated Markdown and dynamic controls too. Keep no-JS ordinary
link fallbacks, and never auto-redirect visitors away to an external site.

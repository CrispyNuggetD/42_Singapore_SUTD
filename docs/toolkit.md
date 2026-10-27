## Take the useful bits.

**Campus HQ web toolkit** · Two MIT-licensed tools you can copy into your own website. The effects on this portfolio are working examples; the code lives in [FOSS Toolbox](https://github.com/CrispyNuggetD/RykerTorch-FOSS-Toolbox). No framework or 42 API account required.

| Tool | What you get | Use it independently? |
| --- | --- | --- |
| **Adaptive Pointer** | The morphing cursor, text feedback and its own on/off switch. | Yes. No astronaut or background needed. |
| **Event Horizon** | Deadline scene, flip countdown, time ruler and profile astronaut; optional travelling pet and chatter. | Yes. Works with the normal OS cursor. |
| ↳ **Space backdrop** | Layered stars, clouds, galaxy glow and occasional comets behind the page. | Optional Event Horizon add-on. Shares its motion switch. |

The **pointer switch** controls the cursor. **Space motion** controls the scene, pet animation/chatter and optional backdrop. They are separate preferences. The course log on this portfolio is supplied by this site's saved project data; the reusable scene does not fetch your Intra history.

### Adaptive Pointer

[Code, demo & copy instructions](https://github.com/CrispyNuggetD/RykerTorch-FOSS-Toolbox/tree/main/web-apps/adaptive-pointer) · [MIT licence](https://github.com/CrispyNuggetD/RykerTorch-FOSS-Toolbox/blob/main/web-apps/adaptive-pointer/LICENSE)

Copy `adaptive-pointer.css` and `adaptive-pointer.js` into your site, then include:

```html
<link rel="stylesheet" href="adaptive-pointer.css">
<script defer src="adaptive-pointer.js"></script>
```

### Event Horizon: the black-hole widget

[Code, markup, demo & full instructions](https://github.com/CrispyNuggetD/RykerTorch-FOSS-Toolbox/tree/main/web-apps/event-horizon) · [MIT licence](https://github.com/CrispyNuggetD/RykerTorch-FOSS-Toolbox/blob/main/web-apps/event-horizon/LICENSE)

<details markdown="1">
<summary>Build your own astronaut scene — setup and optional extras</summary>

1. Copy `event-horizon.css`, `event-horizon.js` and the contents of `scene.html` from the source directory. Keep the licence notice.
2. Paste that scene markup where the widget belongs. Set its `data-start`, `data-deadline`, `data-login` and profile link to your own values. Update the visible fallback date too.
3. Include these files:

```html
<link rel="stylesheet" href="event-horizon.css">
<script defer src="event-horizon.js"></script>
```

Dates accept ISO timestamps or Unix seconds/milliseconds. You can supply them manually: **no API required**. An optional minimal JSON feed can update the scene; credentials stay on the server. The source README includes the owner-profile adapter.

For the travelling pet, add `data-dock-astronaut` to the scene. Add `data-astronaut-chatter` for the silly bubbles. Both are optional.

**Want the background too?** Copy `space-backdrop.css` and `space-backdrop.js` from the same directory, add `data-space-background` to the scene, and load the add-on after the scene script:

```html
<link rel="stylesheet" href="space-backdrop.css">
<script defer src="space-backdrop.js"></script>
```

Use a `.wrap` container around your reading content: the add-on keeps it opaque grey while space moves in the margins. You can restyle that surface. Leave out the add-on to keep your existing page background.

The cursor is a separate tool; include its files only if you want that effect too. All animation respects reduced motion; no-JavaScript visitors keep normal links and fallback scene content.

</details>

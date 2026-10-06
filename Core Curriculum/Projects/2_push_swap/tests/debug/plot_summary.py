#!/usr/bin/env python3
"""Plot lgi's Markdown summaries without rerunning the solver.

Usage: python3 tests/debug/plot_summary.py [summary.md] [--output DIR] [--show]
Requires matplotlib and numpy. Defaults to the newest summary with >=2000 runs.
Outputs an overview PNG, a multi-page PDF covering every numeric statistic,
and a lossless JSON snapshot of all tables and source Markdown.
"""
import argparse
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re
import textwrap

import matplotlib
import numpy as np

DEBUG = Path(__file__).resolve().parent
PALETTE = ['#007f86', '#ea8c35', '#5264b5', '#cd5574', '#779d3e', '#986aba']
SHORT = {
    '(BFS Brute-Force Optimum Moves)': 'BFS optimum',
    '(3-element seed + local greedy)': 'Seed + greedy',
    '(Circular LIS + local greedy)': 'LIS + greedy',
    '(Circular LIS + lookahead greedy)': 'LIS + lookahead',
    '(Circular LIS + opening only 1 insertion with extra lookahead greedy)': 'LIS + single opening',
    '(Circular LIS + opening batch moves from extra lookahead greedy)': 'LIS + batch opening',
}


def parse_markdown(source):
    """Preserve every table, including the Session table inside the benchmark section."""
    tables = []
    section = 'Summary'
    lines = source.splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith('## '):
            section = line[3:]
        if line.startswith('|') and i + 1 < len(lines) and re.match(r'^\|[\s:|\-]+\|$', lines[i + 1]):
            headers = [x.strip().replace('**', '') for x in line.strip('|').split('|')]
            rows = []
            i += 2
            while i < len(lines) and lines[i].startswith('|'):
                cells = [x.strip().replace('**', '') for x in lines[i].strip('|').split('|')]
                if len(cells) != len(headers):
                    raise ValueError(f'Malformed table row {i + 1}')
                rows.append(dict(zip(headers, cells)))
                i += 1
            tables.append({'section': section, 'headers': headers, 'rows': rows})
            continue
        i += 1
    return tables


def number(value):
    try:
        return float(value)
    except (ValueError, TypeError):
        return np.nan


def latest_summary():
    paths = sorted((DEBUG / 'results' / 'random_tests').glob('*/*_summary.md'),
                   key=lambda p: p.stat().st_mtime_ns, reverse=True)
    for path in paths:
        match = re.search(r'\| Successful runs \| (\d+) \|', path.read_text())
        if match and int(match[1]) >= 2000:
            return path
    raise ValueError('No summary with at least 2000 successful runs; supply a summary path.')


def table(tables, name):
    return next((t['rows'] for t in tables if t['section'] == name), [])


def values(rows, key):
    return np.array([number(r.get(key)) for r in rows])


def groups(rows):
    """Keep binaries/settings separate: resumed runs need not be comparable."""
    result = defaultdict(list)
    for row in rows:
        config = row.get('Settings JSON', '')
        config_id = hashlib.sha256(config.encode()).hexdigest()[:6]
        label = SHORT.get(row.get('Algorithm'), row.get('Algorithm', 'Winning result'))
        if 'Algorithm' in row:
            label += f" · {row.get('Binary SHA-256', '')[:6]}/{config_id}"
        result[label].append(row)
    return {k: sorted(v, key=lambda r: number(r['Input size'])) for k, v in result.items()}


def style_axis(ax, title, ylabel='', xlabel='Input size'):
    ax.set_title(title, loc='left', fontweight='bold', pad=12)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.grid(alpha=.18)
    ax.spines[['top', 'right']].set_visible(False)


def metric_plot(ax, series, metric, log=False):
    for i, (label, rows) in enumerate(series.items()):
        x, y = values(rows, 'Input size'), values(rows, metric)
        valid = np.isfinite(x) & np.isfinite(y)
        if log:
            valid &= y > 0
        ax.plot(x[valid], y[valid], color=PALETTE[i % len(PALETTE)],
                linewidth=1.4, label=label)
    if log:
        ax.set_yscale('log')
    style_axis(ax, metric, metric)


def render(path, output, show):
    import matplotlib.pyplot as plt
    from matplotlib.backends.backend_pdf import PdfPages

    source = path.read_text()
    tables = parse_markdown(source)
    session = next((t['rows'] for t in tables if t['headers'] == ['Session', 'Value']), [])
    meta = {r['Session']: r['Value'] for r in session}
    winner = sorted(table(tables, 'Winning result by input size'), key=lambda r: number(r['Input size']))
    comparison = table(tables, 'Algorithm comparison')
    raw = table(tables, 'Per-run algorithm data')
    if not winner or not comparison:
        raise ValueError('This summary needs per-size winning and algorithm comparison tables.')
    output.mkdir(parents=True, exist_ok=True)
    snapshot = {'source': str(path.resolve()), 'markdown': source, 'tables': tables}
    (output / 'summary_data.json').write_text(json.dumps(snapshot, indent=2, ensure_ascii=False))
    plt.rcParams.update({'font.family': 'DejaVu Sans', 'font.size': 9,
                         'axes.facecolor': '#ffffff', 'figure.facecolor': '#f3f6fa',
                         'axes.labelcolor': '#344256', 'text.color': '#233247',
                         'axes.titlesize': 11, 'savefig.dpi': 170})
    series = groups(comparison)
    title = f"push_swap  /  {int(meta.get('Successful runs', 0)):,} successful trials"
    subtitle = (f"Session {meta.get('Session prefix', '?')}   •   {meta.get('Status', '?')}   •   "
                f"sizes {meta.get('Minimum size', '?')}–{meta.get('Maximum size', '?')}   •   seed {meta.get('Master seed', '?')}")
    pdf_path = output / 'summary_report.pdf'
    with PdfPages(pdf_path) as pdf:
        fig, axes = plt.subplots(2, 3, figsize=(17, 10), layout='constrained')
        fig.suptitle(title + '\n' + subtitle, fontsize=19, fontweight='bold')
        x = values(winner, 'Input size')
        ax = axes[0, 0]
        ax.fill_between(x, values(winner, 'Moves Min'), values(winner, 'Moves Max'), color=PALETTE[0], alpha=.14, label='Min–max')
        ax.fill_between(x, values(winner, 'Moves P25'), values(winner, 'Moves P75'), color=PALETTE[0], alpha=.28, label='P25–P75')
        ax.plot(x, values(winner, 'Moves Mean'), color=PALETTE[0], label='Mean')
        style_axis(ax, 'Winning moves & observed spread', 'Moves')
        ax.legend()
        metric_plot(axes[0, 1], {'Winner': winner}, 'Total seconds Mean', log=True)
        metric_plot(axes[0, 2], series, 'Moves Mean')
        metric_plot(axes[1, 0], series, 'Seconds Mean', log=True)
        metric_plot(axes[1, 1], series, 'Mean extra moves vs best')
        ax = axes[1, 2]
        ax.bar(x, values(winner, 'Runs'), width=1, color=PALETTE[0])
        style_axis(ax, 'Sampling coverage', 'Successful trials')
        ax.set_ylim(0, max(values(winner, 'Runs')) + 1)
        axes[0, 2].legend(fontsize=6, loc='upper left')
        fig.savefig(output / 'overview.png')
        pdf.savefig(fig)
        if not show:
            plt.close(fig)

        # Each numeric summary column gets a plot, retaining all variability,
        # quantile, coverage, timing, instruction, and resource measurements.
        for section in ('Winning result by input size', 'Algorithm comparison'):
            rows = table(tables, section)
            numeric = [key for key in rows[0] if key != 'Input size'
                       and any(np.isfinite(number(row[key])) for row in rows)]
            grouped = groups(rows)
            for start in range(0, len(numeric), 6):
                fig, axs = plt.subplots(2, 3, figsize=(17, 10), layout='constrained')
                fig.suptitle(section + '\n' + subtitle, fontsize=16, fontweight='bold')
                for ax, key in zip(axs.flat, numeric[start:start + 6]):
                    metric_plot(ax, grouped, key, log='seconds' in key.lower() and 'CV' not in key)
                for ax in list(axs.flat)[len(numeric[start:start + 6]):]:
                    ax.set_visible(False)
                handles, labels = axs.flat[0].get_legend_handles_labels()
                fig.legend(handles, labels, loc='outside lower center', ncol=3, fontsize=7)
                pdf.savefig(fig)
                plt.close(fig)

        # Visualize every numeric per-run measurement; hashes/IDs remain in JSON.
        raw_groups = defaultdict(list)
        for row in raw:
            raw_groups[(row['Algorithm'], row['Binary SHA-256'])].append(row)
        numeric = [k for k in raw[0] if k not in ('Run', 'Generation', 'Algorithm ID')
                   and any(np.isfinite(number(r[k])) for r in raw)] if raw else []
        for start in range(0, len(numeric), 6):
            fig, axs = plt.subplots(2, 3, figsize=(17, 10), layout='constrained')
            fig.suptitle('Individual candidate measurements\n' + subtitle, fontsize=16, fontweight='bold')
            for ax, key in zip(axs.flat, numeric[start:start + 6]):
                for i, ((algo, binary), rows) in enumerate(raw_groups.items()):
                    ax.scatter(values(rows, 'Generation'), values(rows, key), s=3, alpha=.24,
                               color=PALETTE[i % len(PALETTE)], rasterized=True,
                               label=SHORT.get(algo, algo) + ' · ' + binary[:6])
                style_axis(ax, key, key, 'Generation ID (sweeps repeat input sizes)')
                if key in ('Wall seconds', 'CPU seconds'):
                    ax.set_yscale('symlog', linthresh=1e-6)
            for ax in list(axs.flat)[len(numeric[start:start + 6]):]:
                ax.set_visible(False)
            handles, labels = axs.flat[0].get_legend_handles_labels()
            fig.legend(handles, labels, loc='outside lower center', ncol=3, fontsize=7, markerscale=3)
            pdf.savefig(fig)
            plt.close(fig)

        # Include small tables, settings, saved-run links, and the runner's exact
        # interpretation notes as readable text pages. Full large tables in JSON.
        notes = []
        for t in tables:
            if t['section'] not in ('Winning result by input size', 'Algorithm comparison', 'Per-run algorithm data'):
                notes.append(t['section'])
                notes.extend(' | '.join(f'{k}: {v}' for k, v in r.items()) for r in t['rows'])
        notes.append('Executable settings (distinct configurations)')
        notes.extend(sorted({r['Settings JSON'] for r in comparison}))
        notes.append('Source prose and saved-run references')
        notes.extend(l for l in source.splitlines() if l and not l.startswith('|'))
        wrapped = [s for line in notes for s in textwrap.wrap(line, 135, break_long_words=True)]
        for start in range(0, len(wrapped), 48):
            fig = plt.figure(figsize=(17, 10))
            fig.text(.04, .95, 'Session details & interpretation', fontsize=17, weight='bold')
            fig.text(.04, .9, '\n'.join(wrapped[start:start + 48]), fontsize=9,
                     va='top', family='DejaVu Sans Mono', linespacing=1.3)
            pdf.savefig(fig)
            plt.close(fig)
    print(f'Source: {path}\nTrials: {meta.get("Successful runs")}\nCandidate rows: {len(raw):,}')
    print(f'Outputs: {output / "overview.png"}\n         {pdf_path}\n         {output / "summary_data.json"}')
    if show:
        plt.show()


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('summary', nargs='?', type=Path)
    parser.add_argument('--output', type=Path, help='Default: summary directory/plots')
    parser.add_argument('--show', action='store_true', help='Also open the overview in a Matplotlib window')
    args = parser.parse_args()
    if not args.show:
        matplotlib.use('Agg')
    try:
        path = args.summary or latest_summary()
        render(path, args.output or path.parent / 'plots', args.show)
    except (OSError, ValueError) as exc:
        parser.exit(1, f'plot_summary: {exc}\n')


if __name__ == '__main__':
    main()

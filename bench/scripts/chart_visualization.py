#!/usr/bin/env python3
"""Publication-quality chart generation for ORM vs SQLite3 benchmarks."""

import json
import os
import sys
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Tuple, Any

# Publication-quality plot settings
plt.rcParams.update(
    {
        "figure.dpi": 300,
        "savefig.dpi": 600,
        "savefig.bbox": "tight",
        "font.size": 11,
        "axes.titlesize": 13,
        "axes.labelsize": 12,
        "xtick.labelsize": 11,
        "ytick.labelsize": 11,
        "legend.fontsize": 10,
        "font.family": "sans-serif",
        "font.sans-serif": ["Arial", "Helvetica", "DejaVu Sans"],
        "axes.unicode_minus": False,
        "figure.facecolor": "white",
        "savefig.facecolor": "white",
        "axes.edgecolor": "black",
        "axes.linewidth": 0.8,
    }
)
import warnings

warnings.filterwarnings("ignore", category=UserWarning)


def load_results(json_file: str) -> List[Dict[str, Any]]:
    with open(json_file, "r") as f:
        data = json.load(f)
    return data.get("benchmarks", [])


def extract_benchmark_data(
    results: List[Dict[str, Any]],
) -> Dict[str, Dict[str, List[Tuple[int, float]]]]:
    data = {
        "insert": {"sqlite": [], "orm": []},
        "query": {"sqlite": [], "orm": []},
        "update": {"sqlite": [], "orm": []},
        "delete": {"sqlite": [], "orm": []},
    }

    for result in results:
        name = result.get("name", "")
        cpu_time = result.get("cpu_time", 0)

        if "BigO" in name or "RMS" in name:
            continue

        if "/" in name:
            parts = name.split("/")
            if len(parts) >= 2:
                try:
                    batch_size = int(parts[1])
                except ValueError:
                    continue
            else:
                continue
        else:
            continue

        time_ms = cpu_time / 1_000_000

        if "InsertBatch" in name:
            if "SQLite3" in name:
                data["insert"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["insert"]["orm"].append((batch_size, time_ms))
        elif "QueryBatch" in name:
            if "SQLite3" in name:
                data["query"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["query"]["orm"].append((batch_size, time_ms))
        elif "UpdateBatch" in name:
            if "SQLite3" in name:
                data["update"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["update"]["orm"].append((batch_size, time_ms))
        elif "DeleteBatch" in name:
            if "SQLite3" in name:
                data["delete"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["delete"]["orm"].append((batch_size, time_ms))

    for op in data:
        for impl in data[op]:
            data[op][impl].sort(key=lambda x: x[0])

    return data


# Color-blind friendly palette
CLR_SQLITE = "#4C72B0"
CLR_ORM = "#DD8452"
CLR_OVERHEAD = "#55A868"
CLR_EFFICIENCY = "#C44E52"

BAR_ALPHA = 0.85
GRID_ALPHA = 0.2


def _plot_comparison_subplot(ax, op_key, op_name, data, sizes_map):
    op_data = data[op_key]
    if not op_data["sqlite"] or not op_data["orm"]:
        return

    sqlite_map = dict(op_data["sqlite"])
    orm_map = dict(op_data["orm"])

    common_sizes = sorted(set(sqlite_map.keys()) & set(orm_map.keys()))
    if not common_sizes:
        return

    sqlite_times = [sqlite_map[s] for s in common_sizes]
    orm_times = [orm_map[s] for s in common_sizes]

    x = np.arange(len(common_sizes))
    width = 0.32

    bars1 = ax.bar(
        x - width / 2,
        sqlite_times,
        width,
        label="SQLite3",
        color=CLR_SQLITE,
        alpha=BAR_ALPHA,
    )
    bars2 = ax.bar(
        x + width / 2,
        orm_times,
        width,
        label="ORM",
        color=CLR_ORM,
        alpha=BAR_ALPHA,
    )

    overheads = []
    for st, ot in zip(sqlite_times, orm_times):
        if st > 0:
            overheads.append(((ot - st) / st) * 100)
        else:
            overheads.append(0)

    ax2 = ax.twinx()
    ax2.plot(
        x,
        overheads,
        "s--",
        color=CLR_OVERHEAD,
        linewidth=2,
        markersize=7,
        markerfacecolor="white",
        markeredgewidth=1.5,
        label="ORM Overhead (%)",
        zorder=5,
    )

    ax.set_title(op_name, fontsize=14, fontweight="bold", pad=12)
    ax.set_xlabel("Batch Size", fontsize=12)
    ax.set_ylabel("Time (ms)", fontsize=12)
    ax2.set_ylabel("Overhead (%)", fontsize=12, color=CLR_OVERHEAD)

    ax.set_xticks(x)
    ax.set_xticklabels([str(s) for s in common_sizes], fontsize=11)
    ax.grid(True, alpha=GRID_ALPHA, axis="y")
    ax.tick_params(axis="y")
    ax2.tick_params(axis="y", labelcolor=CLR_OVERHEAD)

    lines1, labels1 = ax.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    leg = ax.legend(
        lines1 + lines2,
        labels1 + labels2,
        loc="upper left",
        fontsize=10,
        framealpha=0.9,
    )

    # Overhead labels positioned above markers with dynamic offset
    ref = max(orm_times) if orm_times else 1.0
    offset = ref * 0.04
    for i, oh in enumerate(overheads):
        va = "bottom" if oh >= 0 else "top"
        y_pos = oh + (offset * 1.5 if oh >= 0 else -offset * 3)
        ax2.text(
            x[i],
            y_pos,
            f"{oh:.1f}%",
            ha="center",
            va=va,
            fontsize=9,
            color=CLR_OVERHEAD,
        )

    max_time = max(max(sqlite_times), max(orm_times))
    ax.set_ylim(0, max_time * 1.25)

    max_oh = max(overheads) if overheads else 50
    min_oh = min(overheads) if overheads else 0
    oh_range = max_oh - min_oh if max_oh != min_oh else 50
    ax2.set_ylim(min_oh - oh_range * 0.15, max_oh + oh_range * 0.25)

    sizes_map[op_key] = common_sizes


def create_comparison_charts(
    data: Dict[str, Dict[str, List[Tuple[int, float]]]],
    title_prefix: str = "Performance Comparison",
):
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle(
        f"{title_prefix} - SQLite3 vs ORM",
        fontsize=18,
        fontweight="bold",
        y=0.97,
    )

    operations = [
        ("insert", "Insert", axes[0, 0]),
        ("query", "Query", axes[0, 1]),
        ("update", "Update", axes[1, 0]),
        ("delete", "Delete", axes[1, 1]),
    ]

    sizes_map = {}
    for op_key, op_name, ax in operations:
        _plot_comparison_subplot(ax, op_key, op_name, data, sizes_map)

    plt.subplots_adjust(
        left=0.07, right=0.90, top=0.90, bottom=0.08, hspace=0.30, wspace=0.30
    )
    return fig


def create_overhead_chart(data):
    fig, ax = plt.subplots(figsize=(10, 7))

    operations = ["insert", "query", "update", "delete"]
    op_names = ["Insert", "Query", "Update", "Delete"]

    avg_overheads = []
    for op_key in operations:
        op_data = data[op_key]
        if not op_data["sqlite"] or not op_data["orm"]:
            avg_overheads.append(0)
            continue

        sqlite_map = dict(op_data["sqlite"])
        orm_map = dict(op_data["orm"])
        common = sorted(set(sqlite_map.keys()) & set(orm_map.keys()))

        overheads = []
        for s in common:
            if sqlite_map[s] > 0:
                overheads.append(((orm_map[s] - sqlite_map[s]) / sqlite_map[s]) * 100)

        avg_overheads.append(np.mean(overheads) if overheads else 0)

    bar_colors = [CLR_SQLITE, CLR_ORM, CLR_OVERHEAD, CLR_EFFICIENCY]
    bars = ax.bar(
        op_names, avg_overheads, color=bar_colors, alpha=BAR_ALPHA, width=0.55
    )

    ax.set_title(
        "Average ORM Overhead by Operation Type",
        fontsize=15,
        fontweight="bold",
        pad=15,
    )
    ax.set_xlabel("Operation", fontsize=13)
    ax.set_ylabel("Overhead (%)", fontsize=13)
    ax.grid(True, alpha=GRID_ALPHA, axis="y")

    for bar, overhead in zip(bars, avg_overheads):
        height = bar.get_height()
        offset = 1.5
        ax.text(
            bar.get_x() + bar.get_width() / 2.0,
            height + offset,
            f"{overhead:.1f}%",
            ha="center",
            va="bottom",
            fontsize=13,
            fontweight="bold",
        )

    ax.axhline(y=0, color="black", linewidth=0.8, alpha=0.5)
    ax.axhline(
        y=20,
        color="gray",
        linestyle="--",
        linewidth=1,
        alpha=0.6,
        label="Low (\u226420%)",
    )
    ax.axhline(
        y=50,
        color="gray",
        linestyle=":",
        linewidth=1,
        alpha=0.6,
        label="Moderate (\u226450%)",
    )
    ax.set_ylim(0, max(avg_overheads) * 1.25 if avg_overheads else 100)

    ax.legend(fontsize=11, framealpha=0.9)
    plt.subplots_adjust(left=0.10, right=0.95, top=0.90, bottom=0.10)
    return fig


def _plot_scaling_subplot(ax, op_key, op_name, data):
    op_data = data[op_key]
    if not op_data["sqlite"] or not op_data["orm"]:
        return

    sqlite_map = dict(op_data["sqlite"])
    orm_map = dict(op_data["orm"])
    common_sizes = sorted(set(sqlite_map.keys()) & set(orm_map.keys()))
    if not common_sizes:
        return

    sqlite_per = [(sqlite_map[s] / s) * 1000 for s in common_sizes]
    orm_per = [(orm_map[s] / s) * 1000 for s in common_sizes]

    x = np.arange(len(common_sizes))
    width = 0.32

    bars1 = ax.bar(
        x - width / 2,
        sqlite_per,
        width,
        label="SQLite3",
        color=CLR_SQLITE,
        alpha=BAR_ALPHA,
    )
    bars2 = ax.bar(
        x + width / 2,
        orm_per,
        width,
        label="ORM",
        color=CLR_ORM,
        alpha=BAR_ALPHA,
    )

    changes = []
    for st, ot in zip(sqlite_per, orm_per):
        if st > 0:
            changes.append(((ot - st) / st) * 100)
        else:
            changes.append(0)

    ax2 = ax.twinx()
    ax2.plot(
        x,
        changes,
        "D--",
        color=CLR_EFFICIENCY,
        linewidth=2,
        markersize=7,
        markerfacecolor="white",
        markeredgewidth=1.5,
        label="Difference (%)",
        zorder=5,
    )

    ax.set_title(op_name, fontsize=13, fontweight="bold", pad=10)
    ax.set_xlabel("Batch Size", fontsize=11)
    ax.set_ylabel("Per Item (\u03bcs)", fontsize=11)
    ax2.set_ylabel("Difference (%)", fontsize=11, color=CLR_EFFICIENCY)

    ax.set_xticks(x)
    ax.set_xticklabels([str(s) for s in common_sizes], fontsize=10)
    ax.grid(True, alpha=GRID_ALPHA, axis="y")
    ax.tick_params(axis="y")
    ax2.tick_params(axis="y", labelcolor=CLR_EFFICIENCY)

    lines1, labels1 = ax.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax.legend(
        lines1 + lines2,
        labels1 + labels2,
        loc="upper right",
        fontsize=9,
        framealpha=0.9,
    )

    for i, ch in enumerate(changes):
        color = CLR_ORM if ch > 0 else CLR_SQLITE
        prefix = "+" if ch > 0 else ""
        va = "bottom" if ch >= 0 else "top"
        offset = max(abs(c) for c in changes) * 0.06 if changes else 2
        ax2.text(
            x[i],
            ch + (offset if ch >= 0 else -offset * 2),
            f"{prefix}{ch:.1f}%",
            ha="center",
            va=va,
            fontsize=9,
            color=color,
        )

    ax2.axhline(y=0, color="gray", linestyle="--", linewidth=0.8, alpha=0.5)

    max_val = max(max(sqlite_per), max(orm_per))
    ax.set_ylim(0, max_val * 1.3)

    all_changes = [abs(c) for c in changes if c != 0]
    if all_changes:
        margin = max(all_changes) * 1.5
        ax2.set_ylim(-margin, margin)
    else:
        ax2.set_ylim(-10, 10)


def create_scaling_chart(data):
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle(
        "Per-Item Time Scaling Analysis - SQLite3 vs ORM",
        fontsize=18,
        fontweight="bold",
        y=0.97,
    )

    operations = [
        ("insert", "Insert", axes[0, 0]),
        ("query", "Query", axes[0, 1]),
        ("update", "Update", axes[1, 0]),
        ("delete", "Delete", axes[1, 1]),
    ]

    for op_key, op_name, ax in operations:
        _plot_scaling_subplot(ax, op_key, op_name, data)

    plt.subplots_adjust(
        left=0.07, right=0.90, top=0.90, bottom=0.08, hspace=0.30, wspace=0.30
    )
    return fig


def main():
    try:
        import matplotlib
    except ImportError:
        print("Error: matplotlib is not installed. Run: pip install matplotlib")
        return 1

    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    repo_root = os.path.dirname(project_root)

    result_files = [
        ("comprehensive_results.json", "Memory Database"),
        ("file_benchmark.json", "File Database"),
    ]

    for result_file, title in result_files:
        search_paths = [
            os.path.join(repo_root, "build", "bench", result_file),
            os.path.join(repo_root, "bench", "data", result_file),
            os.path.join(project_root, "data", result_file),
            os.path.join(".", "data", result_file),
        ]
        resolved_path = None
        for path in search_paths:
            if os.path.exists(path):
                resolved_path = path
                break

        if resolved_path is None:
            print(f"Skipping {title}: result file not found")
            continue

        print(f"Processing: {title}")
        print(f"Loading: {resolved_path}")

        try:
            results = load_results(resolved_path)
        except (json.JSONDecodeError, ValueError) as e:
            print(f"Skipping {title}: invalid JSON ({e})")
            continue
        except Exception as e:
            print(f"Skipping {title}: load failed ({e})")
            continue

        print(f"Loaded {len(results)} benchmark results")

        try:
            data = extract_benchmark_data(results)

            fig1 = create_comparison_charts(data, title)
            fig2 = create_overhead_chart(data)
            fig3 = create_scaling_chart(data)

            output_dir = os.path.join(project_root, "charts")
            os.makedirs(output_dir, exist_ok=True)

            filename_base = os.path.basename(result_file).replace(".json", "")

            fig1.savefig(
                f"{output_dir}/{filename_base}_comparison.png",
                dpi=400,
                bbox_inches="tight",
            )
            fig2.savefig(
                f"{output_dir}/{filename_base}_overhead.png",
                dpi=400,
                bbox_inches="tight",
            )
            fig3.savefig(
                f"{output_dir}/{filename_base}_scaling.png",
                dpi=400,
                bbox_inches="tight",
            )

            print(f"Charts saved to: {output_dir}/")
            print(f"  - {filename_base}_comparison.png")
            print(f"  - {filename_base}_overhead.png")
            print(f"  - {filename_base}_scaling.png")

            plt.close("all")

        except Exception as e:
            print(f"Error processing {title}: {e}")
            import traceback

            traceback.print_exc()

    print()
    print("Chart generation complete.")
    print(f"Output: {os.path.join(project_root, 'charts')}/")

    return 0


if __name__ == "__main__":
    sys.exit(main())

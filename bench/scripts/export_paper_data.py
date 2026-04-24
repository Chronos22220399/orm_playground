#!/usr/bin/env python3
"""
Export benchmark data to CSV for paper引用
Generates: mean_time, per_item_time, overhead_percent
"""

import json
import csv
import os
import sys
from pathlib import Path


def load_results(json_path):
    with open(json_path) as f:
        data = json.load(f)
    return data.get("benchmarks", [])


def extract_metrics(results):
    """Extract: operation, implementation, batch_size, mean_time_ns, per_item_ns, overhead%"""
    rows = []
    ops_map = {
        "InsertBatch": "insert",
        "QueryBatch": "query",
        "UpdateBatch": "update",
        "DeleteBatch": "delete",
    }
    impl_map = {"SQLite3": "sqlite3", "ORM": "orm"}

    parsed = []
    for r in results:
        name = r.get("name", "")
        if "/" not in name or any(x in name for x in ("BigO", "RMS")):
            continue
        parts = name.split("/")
        label = parts[0]
        try:
            batch_size = int(parts[1])
        except ValueError:
            continue
        cpu_time = r.get("cpu_time", 0)

        for op_key, op_name in ops_map.items():
            if op_key in label:
                for impl_key, impl_name in impl_map.items():
                    if impl_key in label:
                        parsed.append(
                            {
                                "operation": op_name,
                                "implementation": impl_name,
                                "batch_size": batch_size,
                                "total_time_ns": cpu_time,
                                "per_item_ns": cpu_time / batch_size
                                if batch_size
                                else 0,
                            }
                        )
                        break
                break

    # 计算overhead
    for op in ops_map.values():
        sqlite = [
            p
            for p in parsed
            if p["operation"] == op and p["implementation"] == "sqlite3"
        ]
        orm = [
            p for p in parsed if p["operation"] == op and p["implementation"] == "orm"
        ]
        for s in sqlite:
            o = next((x for x in orm if x["batch_size"] == s["batch_size"]), None)
            if o and s["total_time_ns"] > 0:
                overhead = (
                    (o["total_time_ns"] - s["total_time_ns"]) / s["total_time_ns"] * 100
                )
                s["overhead_pct"] = ""
                o["overhead_pct"] = f"{overhead:.1f}"
            else:
                s["overhead_pct"] = ""
                o["overhead_pct"] = ""

    return parsed


def write_csv(rows, output_path):
    fieldnames = [
        "operation",
        "implementation",
        "batch_size",
        "total_time_ns",
        "per_item_ns",
        "overhead_pct",
    ]
    header_map = {
        "operation": "Operation",
        "implementation": "Implementation",
        "batch_size": "BatchSize",
        "total_time_ns": "TotalTime_ns",
        "per_item_ns": "PerItem_ns",
        "overhead_pct": "ORM_Overhead_%",
    }
    with open(output_path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writerow(header_map)
        for r in rows:
            writer.writerow({k: r.get(k, "") for k in fieldnames})
    print(f"  {output_path}")


def write_latex_table(rows, output_path, title="Benchmark Results"):
    """Generate LaTeX table"""
    ops = ["insert", "query", "update", "delete"]
    op_labels = {
        "insert": "Insert",
        "query": "Query",
        "update": "Update",
        "delete": "Delete",
    }

    latex = []
    latex.append(r"\begin{table}[ht]")
    latex.append(r"\centering")
    latex.append(r"\caption{" + title + "}")
    latex.append(r"\begin{tabular}{lrrrrr}")
    latex.append(r"\toprule")
    latex.append(
        r"Operation & Batch & \multicolumn{2}{c}{SQLite3 (ns)} & \multicolumn{2}{c}{ORM (ns)} \\"
    )
    latex.append(r"\cmidrule(lr){3-4} \cmidrule(lr){5-6}")
    latex.append(r" & Size & Total & Per-item & Total & Per-item & Overhead \\")
    latex.append(r"\midrule")

    for op in ops:
        first = True
        for size in [10, 100, 1000, 10000]:
            sqlite = next(
                (
                    r
                    for r in rows
                    if r["operation"] == op
                    and r["implementation"] == "sqlite3"
                    and r["batch_size"] == size
                ),
                None,
            )
            orm = next(
                (
                    r
                    for r in rows
                    if r["operation"] == op
                    and r["implementation"] == "orm"
                    and r["batch_size"] == size
                ),
                None,
            )
            if not sqlite or not orm:
                continue
            op_display = op_labels[op] if first else ""
            oh = sqlite.get("overhead_pct", "") if sqlite else ""
            if not oh:
                oh = orm.get("overhead_pct", "") if orm else ""
            overhead = oh if oh else ""
            latex.append(
                f"  {op_display} & {size} & "
                f"{sqlite['total_time_ns']:.0f} & {sqlite['per_item_ns']:.1f} & "
                f"{orm['total_time_ns']:.0f} & {orm['per_item_ns']:.1f} & {overhead} \\\\"
            )
            first = False
        latex.append(r"\midrule")

    latex.append(r"\bottomrule")
    latex.append(r"\end{tabular}")
    latex.append(r"\label{tab:benchmark_results}")
    latex.append(r"\end{table}")

    with open(output_path, "w") as f:
        f.write("\n".join(latex) + "\n")
    print(f"  {output_path}")


def main():
    script_dir = Path(__file__).parent
    project_root = script_dir.parent  # bench/
    repo_root = project_root.parent  # repo root
    data_dir = project_root / "data"

    json_files = list(data_dir.glob("*.json"))
    if not json_files:
        print(f"No JSON files found in {data_dir}")
        return 1

    for json_path in json_files:
        print(f"\nProcessing: {json_path.name}")
        results = load_results(json_path)
        rows = extract_metrics(results)
        rows.sort(key=lambda r: (r["operation"], r["implementation"], r["batch_size"]))

        base = json_path.stem
        csv_path = data_dir / f"{base}.csv"
        write_csv(rows, csv_path)

    print(f"\nDone. Files in {data_dir}/")
    print("  *.csv  - open in Excel/WPS, paste into Word table")
    return 0


if __name__ == "__main__":
    sys.exit(main())

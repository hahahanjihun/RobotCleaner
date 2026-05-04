import json
from pathlib import Path

with open("test_result.json", "r", encoding="utf-8") as f:
    results = json.load(f)

total = len(results)
passed = sum(1 for r in results if r["result"] == "PASS")
failed = total - passed
rate = round((passed / total) * 100, 1) if total else 0

rows = ""
for r in results:
    color = "#d4edda" if r["result"] == "PASS" else "#f8d7da"
    rows += f"""
    <tr style="background:{color}">
        <td>{r['id']}</td>
        <td>{r['name']}</td>
        <td><b>{r['result']}</b></td>
    </tr>
    """

html = f"""
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>RobotCleaner System Test Report</title>
<style>
body {{ font-family: Arial; background:#f5f6fa; margin:40px; }}
.card {{ background:white; padding:24px; border-radius:12px; }}
table {{ width:100%; border-collapse:collapse; margin-top:20px; }}
th, td {{ border:1px solid #ddd; padding:12px; }}
th {{ background:#2f3542; color:white; }}
.summary {{ display:flex; gap:12px; }}
.box {{ background:#f1f2f6; padding:16px; border-radius:8px; flex:1; }}
</style>
</head>
<body>
<div class="card">
<h1>RobotCleaner System Test Report</h1>

<div class="summary">
<div class="box"><h3>Total</h3><p>{total}</p></div>
<div class="box"><h3>PASS</h3><p>{passed}</p></div>
<div class="box"><h3>FAIL</h3><p>{failed}</p></div>
<div class="box"><h3>Success Rate</h3><p>{rate}%</p></div>
</div>

<table>
<tr>
<th>TC ID</th>
<th>Test Case</th>
<th>Result</th>
</tr>
{rows}
</table>
</div>
</body>
</html>
"""

Path("reports").mkdir(exist_ok=True)
Path("reports/report.html").write_text(html, encoding="utf-8")
print("reports/report.html generated successfully.")
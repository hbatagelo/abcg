#!/bin/bash
set -euo pipefail

#cd public

echo "Open http://localhost:8080/"

# Serve files using a webserver
# emrun --no_browser --port 8080 public/
python -m http.server -d public 8080

from pathlib import Path

project = "CONFIGURED"
author = "Reece Holland"
release = "0.5.0"

extensions = ["breathe"]

breathe_projects = {
    "CONFIGURED": str(Path(__file__).parent / "_doxygen" / "xml"),
}
breathe_default_project = "CONFIGURED"

exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

html_theme = "sphinx_rtd_theme"
html_title = "CONFIGURED Documentation"

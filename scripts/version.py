#   Derives FIRMWARE_VERSION from git at build time.
#
#   Tagged commit  ->  the tag verbatim, e.g. "v0.2.0"
#   Anything else  ->  "branch-hash",   e.g. "master-affa81b"
#   Detached HEAD  ->  the hash alone (CI checkouts, bisect)
#
#   A "-dirty" suffix marks a build made from an uncommitted tree.

import os
import subprocess

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")


def git(*args):
    # Returns stripped stdout, or None if git is missing or the command fails
    # (no repo, no tags, shallow clone).
    try:
        output = subprocess.check_output(
            ["git"] + list(args),
            cwd=PROJECT_DIR,
            stderr=subprocess.DEVNULL,
        )
    except (subprocess.CalledProcessError, OSError):
        return None

    return output.decode("utf-8", "replace").strip()


def firmware_version():
    # An exact tag match means this commit *is* a release, so the tag alone is
    # the whole version. Anything else is a development build and needs the
    # hash to be traceable.
    tag = git("describe", "--tags", "--exact-match")

    if tag:
        version = tag
    else:
        commit = git("rev-parse", "--short", "HEAD")

        if not commit:
            # No git, or no commits yet. Better to say so than to ship a
            # plausible-looking version that means nothing.
            return "unknown"

        branch = git("rev-parse", "--abbrev-ref", "HEAD")

        # "HEAD" means detached, which is how CI and bisect check out. GitHub
        # still knows which branch it is building, so prefer that over losing
        # the name; locally there is nothing to fall back to but the hash.
        if branch == "HEAD":
            branch = os.environ.get("GITHUB_REF_NAME")

        if branch:
            version = "{}-{}".format(branch, commit)
        else:
            version = commit

    # Tracked-file changes only. Untracked files would flag every scratch file
    # as dirty; drop the -uno if you want new sources to count too.
    if git("status", "--porcelain", "-uno"):
        version += "-dirty"

    return version


version = firmware_version()

print("FIRMWARE_VERSION: {}".format(version))

# StringifyMacro handles the quoting, which is what the escaped -D in
# platformio.ini was doing by hand.
env.Append(CPPDEFINES=[("FIRMWARE_VERSION", env.StringifyMacro(version))])

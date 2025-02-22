# Changelog for break\_interval

## Upcoming Changes

## Version 1.3.4

Fix Debian build in workflow/action. This means that the debian-built releases
should be fixed now.

## Version 1.3.3

Minor tweak to action/workflow for release assets.  
Compiler hardening flags, fixes related to flags.  
Internal change: make flag used by signal "volatile" (doesn't change much for
users).

## Version 1.3.2

Updated action/workflow to build for Debian aarch64.

Minor fixes to action/workflow.

## Version 1.3.1

Updated action/workflow to build for Debian x86_64.

Minor fixes to action/workflow.

## Version 1.3

More tweaks to README.

Allow playing internal file even if custom player command is specified.  
Do this by specifying "INTERNAL_FILE" as the filename parameter.

## Version 1.2

Minor tweaks to help text, README.

Add workflow to build break\_interval for x86_64 and aarch64.

## Version 1.1

Change usage of internal audio to use a Unix pipe instead of reading from
temporary file.

Implement way to run different audio-player with support for custom args.

Some refactorings.

## Version 1.0

Initial version with working program.

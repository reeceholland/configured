Release Packaging
=================

Release builds should be created from the exact commit that will be tagged and
published on GitHub.

Windows Bundle
--------------

Build the Windows installer bundle from a Visual Studio Developer PowerShell:

.. code-block:: powershell

   .\installer\build_msi.ps1
   .\installer\build_bundle.ps1

The bundle output is written to:

.. code-block:: text

   installer\bin\x64\Release\Configured.Bundle.exe

For GitHub releases, rename the bundle to a versioned asset name and zip it if
GitHub rejects direct ``.exe`` upload:

.. code-block:: text

   Configured-Setup-0.5.0-windows-x64.exe

   Configured-Setup-0.5.0-windows-x64.zip

Ubuntu/Debian Package
---------------------

Build the Debian package on Ubuntu with:

.. code-block:: bash

   cmake --preset linux-release
   cmake --build out/build/linux-release
   cpack --config out/build/linux-release/CPackConfig.cmake

The generated ``.deb`` package is written under ``out/build/linux-release``.

Release Checklist
-----------------

Before publishing:

* build the Windows bundle
* confirm installer staging does not include test artifacts
* confirm runtime DLLs are present
* build the Linux release package
* run tests on supported platforms
* tag the release commit
* upload release assets to GitHub
* mark early tester builds as pre-releases until they have been smoke tested

Development
===========

Requirements
------------

Common requirements:

* CMake 3.21 or newer
* Ninja
* C++17 compiler
* Qt 6 Core, Gui, and Widgets
* Git

Windows builds use the Visual Studio MSVC toolchain and the CMake presets in
``CMakePresets.json``. The default Windows presets expect Qt at
``C:/Qt/6.10.1/msvc2022_64`` and vcpkg at ``C:/dev/vcpkg``.

Ubuntu/Debian dependencies:

.. code-block:: bash

   sudo apt-get update
   sudo apt-get install -y build-essential cmake git ninja-build qt6-base-dev

Build Commands
--------------

Windows debug, from a Visual Studio Developer PowerShell:

.. code-block:: powershell

   cmake --preset x64-debug
   cmake --build out/build/x64-debug

Windows release:

.. code-block:: powershell

   cmake --preset x64-release
   cmake --build out/build/x64-release

Linux debug:

.. code-block:: bash

   cmake --preset linux-debug
   cmake --build out/build/linux-debug

Linux release:

.. code-block:: bash

   cmake --preset linux-release
   cmake --build out/build/linux-release

Tests
-----

Run tests with:

.. code-block:: bash

   ctest --test-dir out/build/linux-debug --output-on-failure

On headless Linux systems, set:

.. code-block:: bash

   export QT_QPA_PLATFORM=offscreen

Local API Documentation
-----------------------

The repository also keeps a Doxygen configuration for local C++ API docs. If
Doxygen is installed, generate those docs from a configured build tree:

.. code-block:: bash

   cmake --build out/build/linux-debug --target doc

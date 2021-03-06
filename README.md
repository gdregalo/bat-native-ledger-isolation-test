## MS Visual Studio isolated testing/debugging environment for bat-native-ledger project.

It is made to test and debug the bat-native-ledger library in isolated environment excluding interaction with hosting browser application. 
The solution is based on MS Visual Studio solution and projects, and allows using all power of VS IDE.
It includes all bat-native-ledger's dependencies as submodules, bat-native-ledger itself, test driver application and curl library for handling http/https communication. 
The solution has Debug and Release configurations for x86 and x86_64 platforms. 
Some of the dependencies have pre-generated configuration files and minor patches to make code compilable in VS. 
(The config files and patches are located in **'patches'** directory.)

## Branch/commit of each component used in the solution.  

All libraries,including bat-native-ledger are moving forward and their interfaces are changing. The solution is bound to a particular branch/commit of each component.
Switching to a newer commit is possible but will require some work to put it together.

* **bat-native-anonize**:  master/e9f5bca3ed69e3db7223be4eac2d59145a7a83a2
* **bat-native-ledger**: master/693c6d49a58caf36de77a60aee7bf23fe51162a0
* **bat-native-rapidjson**: master/86aafe2ef89835ae71c9ed7c2527e3bb3000930e
* **bat-native-tweetnacl**: master/1b4362968c8f22720bfb75af6f506d4ecc0f3116
* **bip39wally-core-native**: master/e5aba371a56d3e41f7e80e868312446ce7bd434c 
* **boringssl**: master/0080d83b9faf8dd325f5f5f92eb56faa93864e4c 
* **curl**: master/7212c4cd607af889c9adc47030a84b6f8ac3b0f6 
* **leveldb**: windows/ad834a20a651ebcabf7c03a88712e780a965d4e3 
* **snappy**: master/4f7bd2dbfd12bfda77488baf46c2f7648c9f1999 
* **SqliteModernCpp**: master/1937817c1a6d9bcca19bf6e705cf6df4a1fe404d
* **sqlite**: version 3.24.0

Switching to a new commit will also require updating this **README.md**.

## Prerequisites:

* The **leveldb** for windows depends on [Boost C++ library](https://www.boost.org/). The easiest way is to install pre-built Windows binaries and source files from [Boost download page](https://www.boost.org/users/download).
Configurations for **x86** or **x86_64** will require the matching set of libraries. Tested with [boost_1_67_0-msvc-14.1-32](https://sourceforge.net/projects/boost/files/boost-binaries/1.67.0/boost_1_67_0-msvc-14.1-32.exe/download) and
[boost_1_67_0-msvc-14.1-64](https://sourceforge.net/projects/boost/files/boost-binaries/1.67.0/boost_1_67_0-msvc-14.1-64.exe/download). 
The location of Boost include directory and libs can be configured in projects properties files **globals.props**, **x86.props** and **x86_64.props** (in 'msvcpp' directory) in variables **$(Boost)** and **$(BoostLib)** (set to **'C:\lib\boost_1_67_0\'** by default).

## Initial setup:

To bring all the dependencies, to switch to the right commit and to apply patches please run `pull-depends.bat` from the repository root directory. 

## Testing:

* The bat-native-ledger library is using threads to run tasks and callbacks. 
Use **BraveRewardsService::TestingJoinAllRunningTasks()** call after each test to wait for all spawned tasks/callbacks.

* **LedgerImpl::RefreshPublishersList** is using recursive timer to download a fresh verified publishers list. In this case there will always be a timer waiting task running in background 
and **BraveRewardsService::TestingJoinAllRunningTasks()** will wait forever. Call **BraveRewardsService::AllowTimersRun(number_of_timers)** (set to **1** by default) before **BraveRewardsService::Init()**
to allow a maximum number of timers iterations to run. 

* If you have to run your testing again and again, then don't forget to delete files **publisher_state**, **publishers_list**, **ledger_state** from your home directory.


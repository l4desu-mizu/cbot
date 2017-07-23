# cBot

Yet another chat bot...
(Ideally) Designed to be extensible and connect to different services:
 - Mumble

DAYMN! What a List I got there!

## Bot Types

Currently not very Botty. But you might get the idea reading the code and hopefully docs...

### SimpleMumbleBot
The SimpleMumbleBot will greet everyone in every channel and send a personal message.
After that the only thing he will react to is "time" printing his current time...

### HttpMumbleBot
The HttpMumbleBot uses an extern Http server, sends the messages he receives there and responds with the answer he gets...
This Bot uses Post messaging in the form:

``command=[some message]&username=[from user]&private_message=[private or channel?]``

(Wasn't my idea but hey you see we might be able to plug this thing into anything!)


## Prerequisite (Tools)
 - cmake (3.1 minimum)
 - gcc (6.3.0 minimum)
 - git (obviously...)
 - protobuf-compiler (protoc 3.0.0 minimum)

## Dependencies (Libs)
 - libprotobuf (see protobuf-compiler)
 - libconfig (1.5 minimum)
 - openssl (1.1 I guess)
 - curl (7.52.1 minimum)

Runs on Debian 9 and ArchLinux. (for me that is...)

## Download and Build
 As usual with cmake Projects...
 ```
 git clone https://github.com/l4desu-mizu/cbot.git
 mkdir cbot/build
 cd cbot/build
 cmake ..
 make
 ```

## Run
 Presumably you are in the ``build`` directory described above:
 ```
 ./bin/cbot ../example.cfg
 ```
 Al hail dem errors.

## TroubleShooting
 Small sections with configuration errors that might occur.
### Project
 - it might occur that the project is upgraded and there are old protobuf generated files
 	- simplest solution is to delete the build dir and reinit the project
### Running
 - FileIO errors usually mean that files (as the configfile) could not be found

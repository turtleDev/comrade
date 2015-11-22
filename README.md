#Comrade
A command line utility that notifies you when your Internet is up.

The current build only works for linux, but windows support is on the way.

##Dependencies
* jsmn ( lightweight JSON parser for C )
* SDL2 ( with SDL2 mixer extension )
* libnotify ( a passive notification library) [ linux/unix ]

#Configuration

Comrade will store configuration files in ~/.local/Comrade/config.json
or (if ~/.local does not exist) in ~/.comrade.json. 

If the no HOME variable is set, then comrade does not try to load 
or save any configuration files.

When you run comrade for the first time, it will create the default
configuration file for you. Be sure to modify it!


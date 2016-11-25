#Comrade
<strike>A command line utility that notifies you when your Internet is up.</strike>

<strike>The current build only works for linux, but windows support is on the way.</strike>

A daemon that notifies any change in internet connectivity.

##Roadmap

* integrate `bstrlib` and `glib`, stop raw doging everything.
* add support for windows and mac.

##Dependencies
* SDL2 ( with SDL2 mixer extension )
* libnotify ( a passive notification library) [ linux/unix ]

##Configuration

Comrade will store configuration files in ~/.config/Comrade/comrade.cfg.

If the no HOME variable is set, then comrade does not try to load 
or save any configuration files.

When you run comrade for the first time, it will create the default
configuration file for you. Be sure to modify it!


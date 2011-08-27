OPyHoldem README file

by Satake (satakecode@gmail.com)
http://code.google.com/p/opyholdem/

Ago 26, 2012

== About OPyHoldem ==
----------------------------
OPyHoldem is a dynamic library (dll) for OpenHoldem?.
Now you can run all your bot logic with Python.

OPyHoldem is fast, using native C python api for integration, and runs it's own 
interpreter instance for every OpenHoldem? session.

The code might have some hint's from AdamM's openholdem-xmlrpc-dll
since i first used his lib but found it a bit slow getting symbols and incompatible 
with multiple sessions (it could work with some alterations).
At this point i decided to take my own route and make a Python C API integrated dll 
that called python methods like openholdem-xmlrpc-dll.


=== How to use? ===
----------------------------
1º You will need to place the OPyHoldem dll file (OPyHoldem.dll) in your OpenHoldem? dir

2º Create a folder named "python" inside OpenHoldem? dir (ex: C:\OpenHoldem?\python), 
here you can place all your python scripts to be opened by OPyHoldem.
(Don't worry, i provide sample script (look below) so that you can easily check how things work)

3º You must have installed Python 2.7.2 or later in your system, get it from here:

    http://www.python.org/ftp/python/2.7.2/python-2.7.2.msi

4º Create your python class for OPyHoldem.
The main class must be named "Main", that is the name OPyHoldem will call.
OPyHoldem supports: .py, .pyo, .pyc

=== Sample Script ===

#
# You can use the OpenHoldem module to retrieve symbols from OpenHoldem
# OpenHoldem.getSymbol({int:Seat}, {string:symbol})
#

import OpenHoldem

#
# You can set the name of your bot in __BotName__,
# this will help you identify your bot in the console
# since you can load any script for different profiles
# within different sessions
#

__BotName__ = "PokyBoty - The sample that call's"

#
# Your main class MUST BE CALLED "Main"
#

class Main: 
    
    def __init__(self):
        print "Loaded script :: %s" % __BotName__

    def __del__(self):
        print "Unloaded script :: %s" % __BotName__

    #
    # def _eval(self, cmd)
    #
    # This function is used for debugging proposes, in case you want 
    # to be able to call class members from the console.
    #

    def _eval(self, cmd):
        exec cmd
    
    #
    # def process_message(self, message, s)
    #
    # Here is where all the magic appends, OPyHoldem will call this function
    # for every action on OpenHoldem, to respond you can return 0 to deny
    # the action or return 1 to accept it.
    # In this example the bot will always call.
    #
    # The "state" message sends in the second argument one Dictionary with all the 
    # games state:
    #
    #{
    #        'm_cards': [0, 0, 0, 0, 0], 
    #        'm_is_playing': 0, 
    #        'm_pot': [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0], 
    #        'm_dealer_chair': 0, 
    #        'm_title': 'Poker Academy Pro', 
    #        'm_fillerbits': 0, 
    #        'm_fillerbyte': 0, 
    #        'm_is_posting': 0,
    #        'm_player': [
    #                { 
    #                        'm_currentbet': 0.0, 
    #                        'm_cards': [0, 0], 
    #                        'm_balance_known': 1, 
    #                        'm_balance': 203.5, 
    #                        'm_fillerbits': 0, 
    #                        'm_fillerbyte': 0, 
    #                        'm_name_known': 1, 
    #                        'm_name': 'Saxum'
    #                }, {
    #                        'm_currentbet': 0.0, 
    #                        'm_cards': [0, 0], 
    #                        'm_balance_known': 1, 
    #                        'm_balance': 179.0, 
    #                        'm_fillerbits': 0, 
    #                        'm_fillerbyte': 0, 
    #                        'm_name_known': 1, 
    #                        'm_name': 'Erasmus'
    #                } ... # The players list always have 10 elements
    #        ]
    #}
    

    def process_message(self, message, s):

        if(message == 'state'):
            self.players = s['m_player']
            
            return 0

        elif(message == 'query'):
            if s == 'dll$iswait':
                print 'Print OpenHoldem is waiting for me!'
                print 'handsplayed: %f'% OpenHoldem.getSymbol(0, 'handsplayed')
                print 'Session: %f' % OpenHoldem.getSymbol(0, 'session')
                print 'done!'
                return 0

            if s == 'dll$rais':
                print 'Should I rais? YEEESSS!!!'
                return 1 # if want raise, return 1

            if s == 'dll$call':
                print 'Should I call?'
                return 0 # if want call, return 1
            

            if s == 'dll$prefold':
                print 'Should I fold?'
                return 0 # if want fold, return 1
        
            return 0


5º Load the dll within OpenHoldem? and open your module with the first tool-bar button.

=== The Tool-bar ===
----------------------------
[1º] [2º] [3º] | [4º]

1º Open Module (the module must be within the python folder you created inside the OpenHoldem? folder)

    Ex: "C:\OpenHoldem?\python\YourBot?.py"


2º Reload Module / Restart Module (You can reload your module if you have made any changes to the source 
or you can restart it after the stop button been clicked).

3º Stop module (Stops the running python module, you can restart it by clicking in the Restart button (2º))

4º About/License (I think this one speaks for it self)

=== Gui command line ===
----------------------------
At the bottom of the OPyHoldem Gui you can see one little input-box, here you can call many commands for debugging.
If you type !h or !help you will get the simplest of the manuals:

############   OPyHoldem   ############
#                                     #
# Ask for OpenHoldem symbols          #
# ${Symbol}[, chair Nº]               #
#                                     #
# Ex: $handsplayed                    #
#-------------------------------------#
# Running module filename and name    #
# !b (!bot)                           #
#-------------------------------------#
# OPyHoldem and Python versions       #
# !v (!version)                       #
#-------------------------------------#
# Calls your module self.stats()      #
# !s (!stats)                         #
#-------------------------------------#
# You can call any member of your     #
# module just by calling it's name    #
# or any valid python syntax.         #
#                                     #
# You need the following method in    #
# your python class to use this:      #
#                                     #
# def __eval(self, cmd):              #
#     exec cmd                        #
#######################################
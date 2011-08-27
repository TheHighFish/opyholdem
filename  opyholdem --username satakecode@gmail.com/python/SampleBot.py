#############################################################################
#  OPyHoldem - Python dll module for OpenHoldem - Sample python script      #
#  =======================================================================  #
#  Copyright (C) 2011           Satake ' <satakecode@gmail.com>             #
#                                                                           #
#    This program is free software: you can redistribute it and/or modify   #
#    it under the terms of the GNU General Public License as published by   #
#    the Free Software Foundation, either version 3 of the License, or      #
#    (at your option) any later version.                                    #
#                                                                           #
#    This program is distributed in the hope that it will be useful,        #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#    GNU General Public License for more details.                           #
#                                                                           #
#    You should have received a copy of the GNU General Public License      #
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.  #
#                                                                           #
#    http://code.google.com/p/opyholdem/                                    #
#                                                                           #
#############################################################################

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

__BotName__ = "PokyBoty - The bot that rocks"

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
    #                } ... # The players list has always 10 elements
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
MUDB+�0�;�0� include/config.h include/runtime_config.h include/command.h include/mudlib.h include/dirs.h include/net/daemons.h include/dirs.h  adm/cmd/player/locate.c�  �         T   f   @&  & �����         \  d  �  ����        6   	    < h$ �/ �-	�
�@q-.@q.l:  % ��          % 8�         % �"         % H!       % h!        % h"       e h!         �           � -      � �� �� h� �( (( H� �x (� �� X� �9 �� � � � �1 �   �      �                    --Hammy

 l, users, who
          �        obj/inherit/base/daemon.c obj/inherit/clean_up.c adm/cmd/player/locate.c include/config.h include/runtime_config.h include/command.h include/mudlib.h include/dirs.h include/net/daemons.h   Locate whom?
 /obj/daemon/net/services   eventSendLocateRequest system Locate query sent.
 help\Syntax: <locate [person]>

This command allows you to know on which MUDs connected through the Intermud 3 network have someone using the name you specify.  Note that this person may not actually be the person you think it is, as nothing prevents two different people from using the same name on different MUDs.

See also: mail, mudlist, rwho, tell
 no_clean create remove Destruct query_prevent_shadow clean_up
 SetNoClean clean_up main helpO O      	  m    
      #          #       !	,  
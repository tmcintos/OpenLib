MUDB�<�0�;�0� include/config.h include/runtime_config.h include/command.h include/mudlib.h include/dirs.h include/net/daemons.h include/dirs.h  adm/cmd/player/tell.c�  ��         T   ;   @#  # ����H  �  �  �  �  �  �  ����        �  
 k   <  �-]�=Zt=Z� @�B	
�]�B A�A_<F=C\A�B	=Zt]]G @��F	�]�- <]�A^< �A^�A_<F	] 
]	 " H �
 �-< ]<h$ </� �	=Zt]]]K @��F		
�=Z�=Z- <]�A^< �A^�A_<F	] 
]	 " L < <
 �-
 �^�-�
 �- ���^-<� q]
�
 �-� <�% <�h �^qc <�% <�
<4 �^qB � �^^q-  <�$ <� �^^q�^<^^<�^ ^<^^q-.!"@q.#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijH ]
]< t]]. ���F<B ���F<B	^=[ ���F	^=[" / <-.  % ��          % 8�         % �"         % H!       % h!        % h"       e h!       
 �           � -      �; 6     �� �� �� �� �( (( H� Ȣ �� X� �x H H< � H� H� �� xp �8 � h� � �� �� h� � h� h� � H� (9 �8 �� � � ( x9 �; �7 �7 �7 �7 �7 � �� x< �7 x7 h7 X7 H7 ( �� 87 (7 �? 7 h 7 �6 �6 � �6 �6 �6 �6 �6 �6 x6 h6 X6 � 8� H6 � 86 (6 8? 6 x 6 �5 �5 �5 �5 �5 �5 �5 �5 x5 h5 X5 H5 85 (5 5 5 �4 �4 �4 �4 �4 �4 �4 �4 x4 h4 �1 �  l      �       l     <      �} !                  �   B   �}  obj/inherit/base/daemon.c obj/inherit/clean_up.c adm/cmd/player/tell.c include/config.h include/runtime_config.h include/command.h include/mudlib.h include/dirs.h include/net/daemons.h Syntax: <tell [who] [message]>
 @     /obj/daemon/net/intermud
 GetMudName Tell whom what?
  is not aware of that mud.
 What do you wish to tell?
 /obj/daemon/net/services 
 eventSendTell query_cap_name short	 my_action  is net-dead.
0  is idle andmay not have been paying attention.
 tell
 GetBlocked  is blocking  all tells.
  is in input   and may not be able to respond.
  tells you: 	 You tell  :  help�Syntax: <tell [player] [message]>
        <tell [player]@[mud] [message]>

Sends the message to the player named either on this mud if no mud is specified, or to the player named on another mud when another mud is specified.  For muds with more than one word in their names, use . (periods) to take place of spaces.  Example: tell descartes@realms.of.chaos hi

See also: say, shout, yell, emote
 a .- b -... c -.-. d -.. e . f ..-. g --. h .... i .. j .--- k -.- l .-.. m -- n -. o --- p .--. q --.- r  .-. s ... t - u ..- v ...- w .-- x -..- y -.-- z --.. 1 .---- 2 ..--- 3 ...-- 4 ....- 5 ..... 6  -.... 7 --... 8 ---.. 9 ----. 0  ----- no_clean create remove Destruct query_prevent_shadow clean_up
 SetNoClean clean_up main help morse� �      	  m    
      #          #    m  
$%&'()*,./ 0145	689:;<= >?BCDEGIM	P
QRTV	UVWY	XYZ\[	\^`_`bdcdsrz������  
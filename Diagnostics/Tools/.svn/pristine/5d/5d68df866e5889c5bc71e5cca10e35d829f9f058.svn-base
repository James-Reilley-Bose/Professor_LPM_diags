#!/bin/bash

# 16.04 deps
# minicom vim cifs-utils nfs-kernel-server python-rbtools gupnp-tools - extra utilities
# subversion - needed to checkout shelby
# git - needed to checkout kernel
# u-boot-tools - need to build u-boot
# nfs-common - need to mount /tools/hepdsw
# libc6:i386 libstdc++6:i386 - need to run the SDK installer
# zlib1g:i386 - needed to build the kernel
# astyle - needed by the run_astyle.pl script
# libxml2-utils - need for xmllint once you start building 
# cmake - needed to build
# libssl-dev:i386 - crypto needed to build native
# libdirectfb-dev:i386 - needed to build native
# libnl-3-dev:i386 
# libglib2.0-dev:i386
# libxml2-dev:i386
# libcurl4-gnutls-dev:i386 

# May only be needed for gcc 5.3 support (default compiler)
# libstdc++-5-dev:i386 - needed to build SCMUpdater
# g++-multilib gcc-multilib - to fix "/usr/bin/ld: skipping incompatible /usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.so when searching for -lstdc++"

# python-lxml - create-configs.py script needs this
# python-xmltodict - make-apconfig needs this
# xsltproc - needed by /App/Configs/generate

# gcc-4.7
# g++-4.7
# g++-4.7-multilib

# Homer Deps
# libquazip1
# libavahi-compat-libdnssd1
# libqt5webkit5

# STSDK
# fop 
# openjdk-8-jdk 
# xsltproc

# Riviera
# adb
# fastboot
echo ".......................   ..   .... ..
.............  I7DMMMMMMMMO7?..       
..........NMMMMMMMMMMMMMMMMMMMMMM7. ..
...... .~MMMMMMMMMMMNDDNMMMMMMMMMMM,..
 ..,NNM. ,MMM7..  .......  ...?MMM ...
   .MMMM   ..     ..=??~. .    . ...  
    :MMMM.    MMMMMMMMMMMMMMMZ .      
     ?MMMM  =MMMMMMMMMMMMMMMMMM:    ..
      .MMMM,.$MMM. ..   . ?MMMD.......
        MMMM..      .   .        .. ..
        ?MMMM,  +MMMMMMMMMM~.      .  
        .:MMMM .MMMMMMMMMMMZ.      .  
          :MMMN  MMD::,~MM~ .... .....
 ... ..... ?MMMN . .. ... ............
 ..  .......MMMMM.... ................
 .......MMMMMMMMMM ...................
......,MMMMMMMMMMMM...................
 ... .MMMMMMMMMMMMM.. ................
 ... .MMMMMMMMMMMMM..........   ... ..
       MMMMMMMMMMM,     .....    .....
 ..  .. ~MMMMMMN=. ...  .....  ......."
echo ""
echo "This script will do everything you need to setup ubuntu for Riviera and Shelby"
echo "Only thing you need to do before hand is setup your ssh keys with github/lab"
echo ""
echo "Note: it will take a few runs for everything to complete but just answer n to "
echo "the steps you have previously done. "
echo "  Run 1) [SUDO] Setup scratch and then create user. (Will kick you out and switch to new user for the rest)"
echo ""
echo "  Run 2) [SUDO] Install libraries, config scratch samba share, and mount tools"
echo ""
echo "  Run 3) [NO SUDO] Checkout repos"
echo ""
echo "ENJOY!"
echo ""
echo ""
echo ""

scriptDir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")


read -p "Create scratch? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo mkdir -p /scratch
    sudo chmod 777 /scratch
    cd /scratch
fi

read -p "Create new user? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "You must have created a temp user at install"

    echo "Enter Bose username ie xx12345 :"
    read username
    echo "Enter Bose user # ie 12345 :"
    read usernum
    echo "Enter new account username"
    read funName

    echo "Creating user"
    sudo adduser $funName -u $usernum -gid 121 -home /home/$funName
    sudo usermod -aG sudo $funName

    su $funName
fi

read -p "Install libraries etc? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Installing dependencies..."

    #optional but useful packages
    sudo apt-get install --yes --force-yes samba vim minicom python-rbtools gupnp-tools openssh-client openssh-server

    sudo apt-get install --yes --force-yes subversion git astyle mtd-utils u-boot-tools build-essential cmake libxml2-utils nfs-common nfs-kernel-server cifs-utils libc6:i386 libstdc++6:i386 zlib1g:i386 libstdc++-5-dev:i386 libssl-dev:i386 libdirectfb-dev:i386 libnl-3-dev:i386 libglib2.0-dev:i386 libxml2-dev:i386 g++-multilib gcc-multilib libcurl4-gnutls-dev:i386 libpython2.7-dev libboost-python-dev libpython2.7-dev:i386 g++-4.7 g++-4.7-multilib gcc-4.7 ccache

    # In place of /tools/hepdsw/scripts/install-python-2.7.sh
    sudo apt-get install --yes --force-yes python-psutil python-xmltodict python-requests python-pytest python-protobuf python-pip

    # more newer stuff and qualcomm stuffs
    sudo apt-get install --yes --force-yes adb fop openjdk-8-jdk xsltproc autofs fastboot docbook-xsl
    pip install lxml humanize


    echo "Copying modified fakeroot and backing up old"
    sudo cp /usr/bin/fakeroot-sysv /usr/bin/fakeroot-sysv.bak
    sudo cp $scriptDir/fakeroot-sysv-64bit /usr/bin/fakeroot-sysv

fi


read -p "Config for LpmService autogen" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    cd /scratch
    git clone https://github.com/doxygen/doxygen.git
    cd doxygen
    sudo apt-get install --yes --force-yes flex bison
    # get to 1.8.13
    git reset --hard 5b36add658b1e5ee14acd747ffe2c4ff9c926833
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..
    make
    sudo make install
fi

read -p "Create scratch samba share? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo apt-get install --yes --force-yes samba
    echo "Creating samba share for scratch"
    name = `whoami`
    sudo echo "security = share
    guest account = nobody
    [scratch]
    path=/scratch/
    public=yes
    browsable=yes
    guest ok=yes
    read only=no
    writable=yes
    available=yes
    force user=${name}" >> /etc/samba/smb.conf

    sudo service smbd restart
fi

read -p "Mount tools and softlib? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then

    echo "Mounting tools"
    cp $scriptDir/auto.tools /etc
    echo "/net    -hosts" >> /etc/auto.master
    echo "/tools /etc/auto.tools" >> /etc/auto.master
    sudo service autofs restart

    echo "Mounting softlib"
    sudo ln -s  /net/solid/softlib/ /home/
fi

read -p "Checkout shelby? (can't run as sudo) (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    svn co https://svn.bose.com/hepd/Shelby/trunk Shelby
    cd /scratch/Shelby

    echo "Installing the SDK..."
    TI-HSP/Scripts/install-sdk.sh

fi

read -p "Pull Professor? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Pulling castle tools"
    cd /scratch
    git clone git@github.com:BoseCorp/CastleTools.git

    echo "Pulling Professor"
    cd /scratch
    git clone git@github.com:BoseCorp/Professor.git

    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.profile
    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.bashrc
    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.login
    source ~/.profile
fi

read -p "Pull Eddie? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Pulling castle tools"
    cd /scratch
    git clone git@github.com:BoseCorp/CastleTools.git

    echo "Pulling Eddie"
    cd /scratch
    git clone git@github.com:BoseCorp/Eddie.git

    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.profile
    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.bashrc
    echo "export PATH=$PATH:/scratch/CastleTools/bin" >> ~/.login
    source ~/.profile
fi

echo "Done!"

echo "
    .  .   ........................................................................   ..   .... .......   ........  
    .  .   ... ..........................................................  I7DMMMMMMMMO7?..                         
..  ........... ............ .  ......................................NMMMMMMMMMMMMMMMMMMMMMM7. .......    ......   
..  ...................  ........... ...  . ......... ............ .~MMMMMMMMMMMNDDNMMMMMMMMMMM,......     ...      
.   .    ..    ............................................. ..,NNM. ,MMM7..  .......  ...?MMM .......   ...........
    ....... ... .. ...    .. .       ...                       .MMMM   ..     ..=??~. .    . ...  . ..     .. ..    
..  ...........                                                 :MMMM.    MMMMMMMMMMMMMMMZ .      .        ..       
..  .  . ...    .                      .                         ?MMMM  =MMMMMMMMMMMMMMMMMM:    ..         ...      
 .  .      .                           .    .. ...    .. .        .MMMM,.$MMM. ..   . ?MMMD.........................
 .       ..                     ...................                 MMMM..      .   .        .. ..                  
..  .         .               .,+ZMMO=..=8MNNNDDN?. . ..            ?MMMM,  +MMMMMMMMMM~.      .  . ..       .      
..  .    ...  .           ...ODDDDDDDDDDDDDDDDDDDD8OO....           .:MMMM .MMMMMMMMMMMZ.      .           ...      
 .       ..               .ONDDNNNNNNNNNDDDDDDDDDDDD8ZD..             :MMMN  MMD::,~MM~ .... ...... .....  ...   ...
 .                       ,MMNNNNNNNNNNMMMNNDNDDDDDDDD8:O. .. ... ..... ?MMMN . .. ... ............ .  ...  ...   ...
                .....    .NNNNNNNNNNMI777INNDDDNNDDDDDD..... ..  .......MMMMM.... ..................................
. ........  :+........ .. NNNNNNNNNNI7I+++++++Z?++?DDD8D.... .......MMMMMMMMMM .....................................
  ...8?+$ .???$ ..........NNNNNNNNNN77+++++++++?++++:+DO..........,MMMMMMMMMMMM................................. ...
    .,$?+I.$++O ......    .NNNNNNNNM7I++++?+++++++++~:.$I... ... .MMMMMMMMMMMMM.. ..................................
 ......$++Z=++?....  .   ...7IZMNNNN7++??. ++++++++?+~?., .. ... .MMMMMMMMMMMMM..........   ... .. ......    .   ...
 .++++$+++++++.. ......  ...ZI87MNNN7?I......O++?,...$. . ..       MMMMMMMMMMM,     .....    ..... ...... ...... ...
 .,D??++++++++7.O+~...   ...Z7$O7NNNI?7. 7 ..:++I...  ~  ... ..  .. ~MMMMMMN=. ...  .....  ...................   ...
 ....77++++++++++7,.........,I778II77+I. I ..8++O.Z= .. ............................................................
    ..$I+++++++O.......  ....,I77777I++?=..~+++++?ODI....... ... ............ ... ................    .   ....  ....
    ...ZI7??+++... ...       ..IZ77?+=7++++++?+++$+:$....... ... .......   ...... ..................................
       +OII+ZD,$.I.  .       ...I7II?=Z++Z?+++OZ8?+:..  .... ... ..... .  .... .. ... .............   ... ....   ...
      :~~~~~D+. ..,           . Z777I?+7?+?N7+?+?OI+..  ..   ... ...       ..  .. .........,ZOMNDNM7.....  ...   ...
 .  .8,,,,,    ..Z....    .. ..,77777+?+++?7$ZZZ7Z,......... ... ......... ......... .DO7II77II7IIIIID... ..........
.   .$,,,,,.  .+..7 ..   .  ..7:,7I77?+NZ?+IZNNI$.....  .... ..  .......  .........8Z7I7IIIIIIIIIIIIII8......... ...
    . =,,.:O,.  .. ? .  . ..Z,=,,=777?+ONNNDDDDO~    .... .. .. ... ....   ......?7I7IIIIIIIIIIIIIIIII7M............
      .I,,,,........ ,=+I??+I:=,,.,77I++MONNDDD8.=~.  ..  ..     ..           ..~IIIIIIIIIIIIIIIIIIIIII7~ ..........
       .,,,,,.. ....$.....  .+:, ..,,:+O++?8?M=....~. .......   ................N7IIIIIIIIIIIIIIIIIIII77N:.  .......
       .?,,,,,. ...  ........,,,Z...,:,,,,,~++,I....+...........................DIIIIIIIIIIII77I7I7I$DN$III8........
        .I,,,,,....      .  ..~,,,,7..?,,,,+:~:=Z. 7.,..  ..     ..... .   ..  .DMM8IIIIIIIII7$8NZ7IIIIIII77..... ..
        . 8.,,,,.....  .......=,...I...N7,==Z:~:+.., =, .... ..  ......... .....:ZNOIIIII$DD7IIIIIIIIIIIIN:. .    ..
 .       ..=.,,,,..      .  .~,.........I~:,:7~:,I.   $ ....    ........  .  . .,O7I78MM7IIIIIIII7OMD7IM$Z..........
           ..+~,,,,,... .....=,...I.    .$:,,::O,=.  .==.....   ....  ..  ......:MZ7DDO88M88O$I???ZNO7+??$..........
               .O=.,,,,,,,,$,,,...,+.. ...Z.,~7:I,~...~+ .....................M77M7IIIIIII??????????Z?????N ..   ...
 .              ......:+~:,,,,,,...,:,.....I,~O~=:I  .~.,....... .......  .~N7N7INIIIIIID????????????N????+O .   ...
               ....  ..~,,,,,,,,.....,7 ....+:~:~7:   ZDI ...  . .. ......MM77III$IIIIIZ+???D. . ..I8M7,...O .    ..
                .....  ~,,,,,,,,...  ..7. ..:,~::=,?  ZZ. .. ... .. ....~NIIIIIII7MMMZ8I????? +M  ...M..M...M.  ....
               ........=,,,,,,,,........+. . +~+::II. 8$.?.. ..........~7III77OZ=.N?O?Z?????Z.......,MODN=.~~... ...
 .              .......=,,,,,,,,........ $... :O::?:..Z7.I.. ... .......=$?=......N?OO???????= ....,OD???IMO........
               ....  ..+,,,,,,,,....   ...$...8$~:~,..~.7....  . ...  .   ...  .. :????????Z??INMMM??????7I?..   ...
 .      .      ....  ..?,,,,,,,,,... ..... :...~~::7O.$ $.: .......................ZM7??????????7??????????8 .......
        .       .......I,,,,,,,,,..........,...~?:~+?  =$ Z.......................II8??????????????????????I?.   ...
                .......$,,,,,,,,,..  ...  .....$7~:~~..7Z ~ .  . .......  ........NII???????????????????????M... ...
 ...    .      ....  . $,,,,,,,,,... ..........8:7=~,,..=..Z.  .................. =N???????$DNNNNNN8D8DDDNO:........
               ....  . I,,,,,,,,,. ............7,D88~=..7,..,    ..   .    ..     MM???????I?????+D... .......   ...
               ....... ?,,,,,,,,,, .............7Z==O8..Z ..,................ ... 8778?????????I,...................
               ....... +,,,,,,,,,,..............OZZZZ8  =...,... .. ....  ..... ~$N777O8???????N......... .  ...  ..
 ...            ...   .:,,,,,,,,,,.. ...........8OOOZ$.  I87............  .8D7777NI$77777ZM$??I$Z,.............. ...
               ........:,,,,,,,,,,, ............8OOOO7,. +~I ... .. .....7Z7I777778Z7777777MDNMMI7. .. .. .  .   ...
                .....  ,,,,,,,,,,,,. ...........$OOOO$, .Z~7 ..  .......N7777777777Z$O7777N777I7MMN ............ ...
               ........,,,,,,,,,,,,.............7OZ8ZZ~..O:: ...........87777777777Z77NI87I77777DI+....   .  .  ....
               ....  ..,,,,,,,,,,,,. .......... 7DZOOZ+..+7..   ........NID7777777$7777777777777DI7:................
 .                    :,,,,,,,,,,,,. ......... .7ZOOOO7..,..   ........O????M77777D7777777777777Z77D................
                ......~,,,,,,,,,,,,............ IOOOOOO..  ...........,I??????NDZDI7777777777777Z77O..... .  .    ..
 .              ..  ..+,,,,,,,,,,,,. ..... .    +OOOOOD...  .   ... ..N??????+D777777777777777777MI...........  ....
                .. ...?,,,,,,,,,,,,. .......  ..~OOOOOD... ...........$??????O77777777777777777777OM................
                ... ..I,,,,,,,,,,,,.   ... .   .~ZOOOO8...: .    .. ..7??????N777777777777777777778+=...............
                  . ..I,,,,,,,,,,,,.   ...     .:ZOOOOO...=         ..$??????D777777777777777777777$?O,.............
                ..  ..7,,,,,,,,,,,,.   ...      ,ZOOOO8. .+      .. ..N??????M777777777777777777777N????~ ..........
                ... ..7,,,,,,,,,,,,....... .   ..ZOOOOO...?. ... .. ..,I??????D$7777777777777777777M?I$?NZ......  ..
               .  .  .7,,,,,,,,,,,,. .......    .ZOOOOO.. ?.     ..... 7???????IM$7777777777777777ZI??MNIO..........
                ..    .??,,,,,,,,,,,.  ... .    ,ZOOOOO.$~.. ... .. ....M??????N?====+?7ZO888Z7?=N.O?$?$7N.  .    ..
                          $I.,,,,,,,.  .   .....:ZOOOOZ.. ..           .:?????+D$==============?M....Z7M7 .  .    ..
                         .+O888D?:.,..  ......:ODOZOOOZ.. ..     ..    ..+NNZZ+==================$....:..    .    ..
               ... ..    .:O88888888O888...D8888OOOOOOZ.. ..   . .. .......~+========Z==7N+=======D ..... ..........
                ...    ....88888ZOZOOOOO.. 888888OOOOOZ.. .. ... .. .......++========O..8==========,..........   ...
                ...  ......88888OOOOOOO$...D88888OZOOOZ.. .. ... .. .......7=========Z...$=========N..... ....   ...
               ............O8888OOOOOOO$...D88888OZOOOZ......  .........  .$=========+...=?========+................
                .. ........88888OOOOOOOZ.. D88888OZOOOO.. .. ... .. ....  .O=========+....8=========,.... ...... ...
                ..    .....88888OOOOOOO$...D88888OOOOOO............ .......+=========,....~=========8......  .    ..
                .. ..    ..D8888OOOOOOO$....8NDO8ZOD8IZ..  ..... .. ....  ?=========D.... :+========N ........  ....
               ............D8888OOOOOZO$ ..=OOOOOOZ777777$~........ ......7==========.... .?======?OM,..............
               ............=D8OOZOOOZOO$ ..IOOOOOOOOO777777Z.... .. ....  D?=======$. ... .I8MM888O888DI.    .    ..
               .         ..IOOOO8ND88O$7O,..=O8O88OOOZ777$7$.    ..       88~O8888888D.....O8$7D888888888=.  .   ...
 .             ...       ..?OOOOOOO$777777$.......=8OO$7$$$.   . .. .. .  8D7D888888888O...ID8888888888888....    ..
                  .      ...  IMZOOOOO77777I+.........  .... ... .........~~7NDD8888888DM.. ?Z~.,:======:,7  .    ..
                         .  ......NOOOO7777$7     .  .    ..           .  ..=MZ+,......~M...........................
               ....  ..  .  ..... ..,=$888I...            ..     ..          .  .    . .    ... ......... ....   ..."

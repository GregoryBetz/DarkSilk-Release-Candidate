# **DarkSilk (DRKSLK) Release Candidate for Version 1.0.0.0**

(FOR TESTING PURPOSES ONLY)

DarkSilk Integration/Staging Tree
================================
![DRKSLK logo](https://raw.githubusercontent.com/SCDeveloper/DarkSilk-Release-Candidate/master/src/qt/res/icons/header.png)

**Copyright (c) 2015-2016 Silk Network Developers**

What is DarkSilk?
----------------
* Algorithm: Scrypt
* Coin Suffix: DRKSLK
* PoW Period: 42,002 Blocks
* PoW Target Spacing: 60 Seconds
* PoW Difficulty Retarget: 10 Blocks
* PoW Reward per Block: 42 DRKSLK
* Full Confirmation: 10 Blocks
* Maturity: 42 Blocks
* PoS Target Spacing: 64 Seconds
* PoS Difficulty Retarget: 10 Blocks
* PoS Reward: 2 DRKSLK Static PoS Reward
* Minimum Confirmations for Stake: 420 Blocks
* PoS Min: 4 Hours
* PoS Max: Unlimited
* Stormnode Collateral Amount: 42000 DRKSLK
* Stormnode Min Confirmation: 10 Blocks
* Stormnode Reward: 2 DRKSLK Static Reward
* Budget Reward: 10,000 DRKSLK Static Reward Every 41,091 blocks (~once per month)
* Budget Proposal Fee: 420 DRKSLK, 20 confirmations, First vote eligible in 20 blocks (~21 minutes).
* Total Coins: 90,000,000 DRKSLK
* Block Size: 50MB (50X Bitcoin Core)




DarkSilk is a new digital currency that enables instant payments to anyone, anywhere in the world. DarkSilk uses peer-to-peer technology over I2P/Tor/ClearNet to operate securly with no central authority (centralisation): managing transactions and issuing currency (DRKSLK) are carried out collectively by the DarkSilk network. DarkSilk is the name of open source software which enables the use of the currency DRKSLK.

DarkSilk utilises Stormnodes, Sandstorm and InstantX to provide anonymous and near instant transaction confirmations.

DarkSilk implements Gavin Andresens signature cache optimisation from Bitcoin for significantly faster transaction validation.

DarkSilk uses ShadowChat from Shadow for encrypted, anonymous and secure messaging via the DarkSilk wallet network.

Darksilk includes a completely decentralised marketplace, providing anonymity and escrow services for safe and fast trades.

DarkSilk includes an Address Index feature, based on the address index API (searchrawtransactions RPC command) implemented in Bitcoin Core but modified implementation to work with the DarkSilk codebase (PoS coins maintain a txindex by default for instance). Initialize the Address Index by running the -reindexaddr command line argument, it may take 10-15 minutes to build the initial index.




**Stormnode/Sandstorm Network Information**
Ported Masternodes from Dash, rebranded as Stormnodes and modified to use stealth addresses.
Darksend ported and rebranded as Sandstorm.
Utilisation of InstantX for instant transaction confirmation.
Multiple Stormnodes can be ran from the same IP address on varied ports.


**MainNet Parameters**
P2P Port/Stormnodes = 31000
RPC Port = 31500
Magic Bytes: 0x42 0x04 0x20 0x24


**TestNet Parameters**
P2P Port/Stormnodes = 31750
RPC Port = 31800
Magic Bytes: 0x24 0x20 0x04 0x42


Debian/Ubuntu Linux Daemon Build Instructions
================================================

install dependencies:

	$ sudo apt-get update && sudo apt-get upgrade
    $ sudo apt-get install git build-essential libssl-dev libdb5.3++-dev libminiupnpc-dev dh-autoreconf zip unzip libboost-all-dev make

build darksilkd from git:

    $ git clone https://github.com/SilkNetwork/DarkSilk.git darksilk
    $ cd darksilk/src/secp256k1 && ./autogen.sh && ./configure --enable-module-recovery && make && cd .. && sudo make -f makefile.unix USE_UPNP=0
   
install and run darksilkd daemon:

    $ sudo strip darksilkd && sudo cp ~/darksilk/src/darksilkd /usr/bin && cd ~/
    $ darksilkd

here are a few commands, google for more.

	$ ./darksilkd getinfo
	$ ./darksilkd getpeerinfo
    $ ./darksilkd getmininginfo
	$ ./darksilkd getstakinginfo
	$ ./darksilkd getnewaddresss
	

Debian/Ubuntu Linux Qt5 Wallet Build Instructions
================================================

update and install dependencies:

    $ sudo apt-get update && sudo apt-get upgrade
    $ sudo apt-get install make libqt5webkit5-dev libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools qtcreator zip unzip dh-autoreconf 
      libboost-thread-dev libssl-dev libdb++-dev libstdc++6 libminiupnpc-dev libevent-dev libcurl4-openssl-dev git libpng-dev qrencode libqrencode-dev 
      install build-essential libboost-dev libboost-all-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev 

build darksilk-qt from git:

    $ git clone https://github.com/SilkNetwork/DarkSilk.git darksilk
    $ cd darksilk && qmake -qt=qt5 "USE_QRCODE=1" "USE_NATIVE_I2P=1" && make
 
running the darksilk Qt wallet:

	$ sudo ./DarkSilk
    
This will compile and build the Qt Wallet which takes a little while, please be patient.  When finished you will have a file called DarkSilk - Simply Double Click


Example stormnode.conf Configuration
===================================================

rpcallowip=127.0.0.1
rpcuser=MAKEUPYOUROWNUSERNAME
rpcpassword=MAKEUPYOUROWNPASSWORD
rpcport=31500
server=1
daemon=1
listen=1
staking=0
port=31000
stormnodeaddr=YOUR_IP:31000
stormnode=1
stormnodeprivkey=KEY GENERATED BY COMMAND
snconflock=1 // Stormnodes can be locked via the configuration file 


Set the stormnodeprivkey to the result of running:

stormnode genkey
//From the RPC console in the local wallet.

//If you are running your stormnode as a hidden service, replace YOUR_IP with your onion hostname (typically found in /var/lib/tor/YOURHIDDENSERVICENAME/hostname).

Port 31000 must be open.

//If you are using the hot/cold system where you are remotely activating your stormnode from your local wallet, you will also put these entries in your local (cold) wallet .conf:

stormnodeaddr=YOUR_IP:31000
stormnode=1
stormnodeprivkey=KEY GENERATED BY COMMAND

//Then, start the daemon on your remote stormnode, start your local wallet, and then you can start your stormnode from the RPC console:

stormnode start


//end of guide

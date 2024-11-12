from mininet.topo import Topo

class MyTopo( Topo ):
    def__init__( self ):
        Topo.__init__(self )
        #Addhostsandswitches
        Host1=self.addHost( 'h1' )
        Host2=self.addHost( 'h2' )
        Host3=self.addHost( 'h3’)
        Switch1=self.addSwitch('s1')
        Switch2=self.addSwitch('s2’)
        Switch3=self.addSwitch(‘s3')
        #Addlinks
        self.addLink( Host1, Switch1 )
        self.addLink( Host2, Switch1 )
        self.addLink( Host3, Switch2 )
        self.addLink( Switch1, Switch2 )
        self.addLink( Switch2, Switch3 )
        topos ={'mytopo': ( lambda: MyTopo() ) }
#  Run the command
#  sudo mn--custom custom_topo.py --topo=mytopo

//Setup Ethernet Interface
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAC };  //used by UIPEthernet
uint8_t serverTS[] ={184,106,153,149};  //ThingSpeak API Address
uint8_t serverLH[] ={10,1,1,25};
uint8_t ip[] = {10,1,1,197};
uint8_t dnsServer[] = {75,75,75,75};
uint8_t gateway[] = {10,1,1,1};
EthernetClient client;

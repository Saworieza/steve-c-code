/ # iptables -- list INPUT
chain INPUT (policy ACCEPT)
target      prot    opt     source                  destination
REJECT      tcp     --      anywhere                anywhere                tcp dpt:http ctstate NEW, ESTABLISH...
HED reject-with     icmp-port-unreachable
ACCEPT      tcp     --      anywhere                anywhere                tcp dpt:ssh ctstate NEW, ESTABLISH...
ED
ACCEPT      tcp     --      anywhere                anywhere                tcp dpt:smtp ctstate NEW, ESTABLISH...
HED
REJECT      udp     --      anywhere                anywhere                tcp dpt:domain ctstate NEW, ESTABLISH...
ISHED reject-with   icmp-port-unreachable
REJECT      all     --      172.17.0.3              anywhere                reject-with icmp-port-unreachable

The rules issued
Default Policy: `chain INPUT (policy ACCEPT): The default policy for the INPUT chain is set to ACCEPT. This means that if a packet does not match any of the explicitly defined rules, it will be accepted by default.

Rejecting HTTP (Web) Traffic: REJECT tcp -- anywhere anywhere tcp dpt:http ctstate NEW, ESTABLISHED reject-with icmp-port-unreachable: This rule rejects incoming TCP traffic on port 80 (HTTP) for new and established connections. 

Accepting SSH (Secure Shell) Traffic: ACCEPT tcp -- anywhere anywhere tcp dpt:ssh ctstate NEW, ESTABLISHED: This rule accepts incoming TCP traffic on port 22 (SSH) for new and established connections.

Accepting SMTP (Email) Traffic: ACCEPT tcp -- anywhere anywhere tcp dpt:smtp ctstate NEW, ESTABLISHED: This rule accepts incoming TCP traffic on port 25 (SMTP) for new and established connections.

Rejecting DNS (Domain Name System) Traffic: REJECT udp -- anywhere anywhere tcp dpt:domain ctstate NEW, ESTABLISHED reject-with icmp-port-unreachable: This rule rejects incoming UDP traffic on port 53 (DNS) for new and established connections.

Rejecting All Traffic from a Specific IP (172.17.0.3): REJECT all -- 172.17.0.3 anywhere reject-with icmp-port-unreachable: This rule rejects all types of incoming traffic from the specific IP address 172.17.0.3 


Q17
a) iptables -A INPUT -p tcp --dport 22 -j ACCEPT
iptables -A INPUT -p tcp --dport 22 -m conntrack --ctstate NEW,ESTABLISHED -j ACCEPT
b) iptables -A INPUT -p udp --dport 53 -j REJECT --reject-with icmp-port-unreachable
iptables -A INPUT -p udp --dport 53 -m conntrack --ctstate NEW,ESTABLISHED -j REJECT --reject-with icmp-port-unreachable
c) iptables -P INPUT DROP
d)  Port 53 (DNS): Status: Open
    Port 22 (SSH): Status: Closed or Filtered
    Port 80 (HTTP): Status: Closed or Filtered
    All Other Ports: Status: Filtered (assuming the default policy for the INPUT chain is DROP)



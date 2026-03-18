*This project has been created as part of the 42 curriculum by hnah.*

# Description

## Born2beroot

**Born2beroot** is a system administration project in the 42 curriculum designed to introduce us to the fundamentals of Linux system configuration, virtualization, and security hardening.

The objective of the project is to create a secure Linux server inside a virtual machine, configure multiple system services, and enforce strict security policies.

# Quick feature list

### Mandatory
In this project I configured:

- a Debian-based virtual machine using VirtualBox
- implemented logical volume management (LVM)
- configured SSH access, enforced password policies
- deployed a firewall, and 
- created a system monitoring script executed through cron.

### Bonus

For the bonus part, I also deployed a full web stack consisting of:

1. Lighttpd (web server)
2. MariaDB (database)
3. PHP (server-side scripting)
4. WordPress (content management system)

This setup simulates a minimal production server architecture.

## Learning Objectives or skills to master through this project

This project teaches the following system administration fundamentals:

- Linux system installation
- Virtualization
- Disk partitioning
- Logical Volume Management (LVM)
- User and permission management
- Secure remote access (SSH)
- Firewall configuration
- Password security policies
- System monitoring with shell scripts
- Scheduled task automation (cron)
- Web server deployment

# Instructions

## Mandatory

For 42 evaluation, BEFORE opening the .vdi that is MADE for the evaluation, run:

```shell
sha1sum <b2br_eval_file_name>.vdi
```

The signature obtained should be the same as the signature.txt in the cloned repository.

Other commands and tests to be done to be based on the evaluation sheet.

## Bonus

For bonus, go to host browser, enter:
http://localhost:6767

(NOTE: http without http(s), i.e. the service does not operate on secure HTTP).

# Explanations of main parts of project

### What's a VM (Virtual Machine)?

A virtual machine (VM) is a software emulation of a physical computer.

- Software emulation is when software replicates the behavior of hardware or another system so that programs can run as if they were using real physical devices.

- It runs on top of a program called a hypervisor (like VirtualBox), which allocates real hardware resources such as CPU, RAM, and disk from the host machine to the VM.

	- A hypervisor is software that allows multiple virtual machines to run on a single physical computer by managing and virtualizing hardware resources.

- The guest operating system (e.g., Debian) believes it is running on real hardware, but the hypervisor intercepts and manages all hardware access.

### Virtualization (Virtual Machine Setup)
The server runs inside a VirtualBox virtual machine.

- Virtualization allows a computer to run multiple operating systems simultaneously by abstracting hardware resources.

#### Advantages:

- Safe experimentation environment

- Isolation from the host system

- Snapshot capability for recovery

- Easy replication and portability

---

### Operating System choice

The operating system used is:

- Debian (stable)

Debian is a popular Linux distribution widely used for servers because it provides:

- Excellent stability

- Strong security maintenance

- Large software repositories

- Reliable package management

Debian uses the APT package manager, which simplifies software installation and dependency management.

Example command:

```c
sudo apt update
sudo apt upgrade
```
---

### Why not Rocky OS?

- More for enterprise

- Less commonly used/ less relevant knowledge

- Less community resources/ support (debugging issues)

- (To be also honest, it'll probably need more time to understand and explore and I don't wanna blackhole in 42, lol oops.)

---

### AppArmor, SELinux, Aptitude, and Apt

#### AppArmor

- AppArmor is a Linux security module that restricts what programs are allowed to do using predefined security profiles.
- It limits things like:
	- which files a program can access
	- which network operations it can perform
- This helps contain damage if a program is compromised.

Important ideas:

1. Programs are kept in a “box”: Each program is limited in what it can do and what parts of the system it can touch

2. Rule lists (profiles): Each program has a list of rules that say what it is allowed to use

3. Checks file locations: AppArmor looks at the file paths a program tries to open and decides if it is allowed

4. Only allowed things work: If something is not listed in the rules, the program is not allowed to use it

5. Extra safety layer: It works together with normal Linux file permissions to make the system safer

#### SElinux

SELinux is a Linux security system that enforces mandatory access control using security policies and labels. It is commonly used in Red Hat–based distributions such as Rocky Linux.

- In SELinux, a label is like a security tag attached to files and programs.

Example:

```shell
File label: database_data
Program label: web_server
```

Rule:

```shell
web_server → cannot access database_data
```

So even if normal Linux permissions allow it, SELinux can still block it.

#### So what's the difference between AppArmor and SELinux?

- AppArmor: controls access based on file paths (which folders or files a program can use).

- SELinux: controls access using security labels attached to files and processes.

Simple difference:

- AppArmor checks where the file is, while SELinux checks what the file and program are allowed to do based on labels.

- Both enforce Mandatory Access Control, but SELinux is more powerful and complex, while AppArmor is simpler to configure.

#### APT
- APT (Advanced Package Tool) is Debian’s package manager used to install, update, and remove software from repositories.
- It handles downloading packages and resolving dependencies automatically.

#### Aptitude

- Aptitude is another package manager that also manages Debian packages but provides an interactive interface and more advanced dependency resolution.
- Today, APT is the standard tool, while aptitude is less commonly used.

#### Summary:

- APT and Aptitude are package managers used to install and manage software in Debian, while AppArmor is a security system that restricts program permissions using security profiles.

---

### So why do people use Aptitude at all?

Why people use aptitude: 

1. Better dependency resolution

When package conflicts occur, aptitude can suggest ***multiple solutions to resolve them***, while **apt usually fails with an error**.

Example situation:

```shell
Package A needs version 1 of library X
Package B needs version 2 of library X
```

```apt``` may simply stop.

```aptitude``` may propose solutions like:

- downgrade package B

- remove package A

- install alternative dependency

2. Interactive interface

Running ```aptitude``` opens a text-based interface where you can browse packages, search software, and manage installations interactively.

3. Smarter dependency cleanup

```aptitude``` can automatically remove unused dependencies more intelligently than older APT versions.

4. However, most people still use apt:

Today apt is preferred because:

- simpler commands

- faster

- default tool in Debian

- covers most use cases

---

### What's ```cron```??

Cron is a Linux service used to schedule commands or scripts to run automatically at specific times or intervals.

- It reads instructions from a configuration file called ```crontab```.

Example:

```shell
*/10 * * * * /usr/local/bin/monitoring.sh
```

Meaning:

- run the script every 10 minutes

### Disk Partitioning & LVM

The project requires the use of Logical Volume Management (LVM).

- LVM provides a flexible way to manage disk space compared to traditional partitioning.

Instead of using fixed disk partitions, LVM creates logical volumes that can be resized, extended, or reduced more easily.

LVM works using three layers:

1. Physical Volume (PV) – the actual disk or disk partition

2. Volume Group (VG) – a pool of storage created from one or more physical volumes

3. Logical Volume (LV) – virtual partitions created inside the volume group

Essentially:

```
Physical disk
     ↓
Physical Volume (PV)
     ↓
Volume Group (VG)
     ↓
Logical Volumes (LV)
     ↓
Filesystems (/home /var /tmp)
```

#### Advantages:

- Dynamic resizing of partitions

- Easier disk management

- Better scalability

- Logical abstraction of storage

The disk is divided into multiple encrypted partitions including:

- ```/```

- ```/home```

- ```/var```

- ```/tmp```

- ```/boot```

This separation improves system stability and security by isolating different types of data.

Example command used to inspect partitions:

```lsblk```

---

### Logical abstraction of storage?

First, what's abstraction? (I forgot, anyway, might as well put it in readme.)

- Abstraction means **hiding messy low-level details behind a cleaner conceptual layer**. 
- A classic example: when we write ```printf()```, we don’t think about CPU registers, syscalls, and device buffers. 
	- Those details are ***abstracted*** away.

And then:

- Storage works the same way.

	- Without abstraction, a filesystem talks directly to physical disk partitions. The structure looks like this:

```
Physical Disk
   ├─ /dev/sda1 → /
   ├─ /dev/sda2 → /home
   └─ /dev/sda3 → /var
```

Each partition is tied rigidly to a specific region of the disk. If ```/home``` runs out of space while ```/var``` is empty, you're stuck. You must **repartition** the disk — risky and painful.

*LVM*, introduces a ***logical layer*** between the filesystem and the physical disk.

- The filesystem no longer cares where the storage physically lives.

So instead of saying:

```“Use disk sectors 20000–50000”```

it says:

```“Give me 20GB of storage.”```

The LVM layer decides which physical disk blocks actually store the data.

- That’s the logical abstraction: the operating system interacts with logical volumes, not raw disk partitions.

- Think of it like virtual memory for disks.

#### Physical memory abstraction:

```
Programs → Virtual Memory → Physical RAM
```

#### Storage abstraction:

```
Filesystem → Logical Volume → Physical Disk
```

#### Another scenario which I learnt from (yes) ChatGPT is:

- Cloud storage

	- When we request 100GB on AWS, we don’t know which hard drive in which data center holds the bits. The infrastructure abstracts that away.

LVM brings a mini version of that idea to a single machine.

---

### Resizing LVMs

We can resize volumes dynamically like this:

```shell
lvextend /dev/vg/home
resize2fs
```

- You can combine **multiple disks into one pool**.

- You can **move data between disks without downtime**.

And the filesystem never notices.

#### Essentially:

- Logical abstraction of storage means the filesystem interacts with logical volumes rather than physical disk partitions. 
- LVM introduces a layer that separates storage management from the physical disk layout, allowing flexible resizing, pooling of disks, and easier administration.

Interestingly, the same abstraction principle appears everywhere in computing:
- Virtual memory abstracts RAM.
- Containers abstract operating systems.
- Hypervisors abstract hardware.
- Computers are basically towers of abstraction layers stacked like "geological strata".

---

### User and Permission Management

Two main users are created:

1. root (administrator)

2. <login> (standard user)

The login user belongs to two groups:

1. sudo
2. user42

Groups allow Linux to manage permissions efficiently.

Example command:

```groups <login>```

---

### Sudo Configuration

sudo allows authorized users to execute commands with elevated privileges without logging in as root.

#### Advantages:

- Improved security

- Activity logging

- Fine-grained privilege control

- Security rules configured:

- Maximum 3 authentication attempts

- Custom error message

- All sudo commands logged

- Logs stored in:

```/var/log/sudo/```

TTY mode (Teletype) was also enabled to ensure sudo commands are executed from a terminal.

Example command:

```sudo visudo```

---

### TTY Mode (```requiretty```)

In sudo configuration we may see something like:

```Defaults requiretty```

This means:

- sudo commands can only run if the user is inside an interactive terminal session.

In other words:

```
User typing commands in terminal → allowed
Script running in background → blocked
Web server executing sudo → blocked
```

This makes it more secure because if requiretty is enabled, attackers cannot easily execute sudo commands from:

- background scripts

- exploited web servers

- automated processes

- remote command injections

**They must have an interactive terminal.**

---

### Password Policy

A strong password policy was implemented using PAM modules.

### PAM is (Pluggable Authentication Modules):

- We need to implement strong assword policy, but to actually enforce those rules on Debian, the mechanism used in Linux is PAM.

- PAM is the Linux framework responsible for authentication and account policies.

- It sits between applications and authentication logic.

- Instead of each program implementing login rules itself, programs ask PAM:

```shell
login
ssh
sudo
passwd
```

Architecture:

```
Application (ssh / login / sudo)
        ↓
        PAM
        ↓
Authentication modules
(password rules, account checks, etc.)
```

So, a PAM module is a small plugin that performs a specific authentication task.

Examples:

Module and Purpose:
- pam_unix.so
	- standard Linux password authentication
- pam_pwquality.so
	- password strength rules
- pam_tally2.so
	- account lock after failed attempts
- pam_limits.so
	- user resource limits

Each module enforces one policy.

- Because PAM is modular, the system administrator can add or remove policies easily.

- That’s why it’s called Pluggable.

---

### Password Rules

Rules enforced:

- Minimum 10 characters

Must contain:

- uppercase letter

- lowercase letter

- number

- Password expires every 30 days

- Minimum 2 days before password change

- Warning 7 days before expiration

- No more than 3 repeated characters

Example configuration file:

```/etc/login.defs```

---

### SSH (Secure Shell)

SSH allows secure remote access to the server. It uses encryption to protect communication between client and server.

Configuration details:

- SSH runs on port 4242 on the guest machine. (Portforwarded to host port 2222)

- Root login is disabled

- Only authorized users may connect

Configuration file:

```/etc/ssh/sshd_config```

Example connection:

```ssh <user>@localhost -p 2222```

### Port forwarding

Port forwarding is used so the host machine can **access a service running inside the virtual machine**.

Because the VM runs on an **isolated virtual network**, the host normally **cannot directly reach its ports**. So, port forwarding maps a port on the host to a port inside the VM.

---

### Isolated virtual network

- A isolated virtual network is a network that exists **only** inside the virtualization system, separated from the real physical network our machine is connected to.

	- When we run a virtual machine in VirtualBox (or similar), the VM gets a virtual network interface card (vNIC). 
	- The hypervisor then connects that virtual NIC to a software-created network rather than directly to your real Ethernet or Wi-Fi.

Essentially:

```
Host computer
   │
   ├─ Real network (your Wi-Fi / LAN / internet)
   │
   └─ Virtual network (created by VirtualBox)
            │
            └─ Virtual machine
```

So, the VM sits inside a **private** network environment that is **logically** separated from the real one. 

- Other machines on your physical network usually cannot see it directly.

The most common mode we use in Born2BeRoot is **NAT networking**. 

- In NAT mode the VM behaves like a device behind a router:

```
Internet
   │
Host machine (acts like router/NAT)
   │
Virtual isolated network
   │
Virtual machine
```

Because of that isolation:

1. The VM can access the internet

2. But external machines cannot directly connect to it

That’s why port forwarding becomes necessary; the hypervisor opens a “tunnel” from the host to the VM so we can SSH into it.

---

### SSH Portforwarding

- Example used in Born2BeRoot:

```
Host machine port: 2222
        ↓
Virtual machine port: 4242 (SSH)
```

So when we run:

```
ssh user@localhost -p 2222
```

the connection goes:

```
Host localhost:2222
        ↓
VirtualBox NAT port forwarding
        ↓
VM port 4242
        ↓
SSH server inside the VM
```

---

### Firewall (UFW)

The firewall used is UFW (Uncomplicated Firewall).

- UFW simplifies Linux firewall management.

Configuration:

- Firewall enabled on startup

- Only port 4242 (Or for other bonus services) allowed

Example commands used:

```shell
sudo ufw enable
sudo ufw allow 4242
sudo ufw status
```

Advantages:

- Easy rule management

- Built on top of iptables
	- iptables is Linux firewall system used to control network traffic.
	- It works by defining rules that decide whether packets are:
		- allowed
		- blocked 
		- redirected
- Suitable for beginner system administrators

---

### Monitoring Script

A system monitoring script was created:

```monitoring.sh```

The script collects important system information and displays it using the wall command.

The information includes:

- OS architecture

- CPU physical cores

- Virtual CPUs

- RAM usage

- Disk usage

- CPU load

- Last boot time

- LVM status

- Active TCP connections

- Logged users

- Network information

- Number of sudo commands executed

- Example snippet:

```shell
arch=$(uname -a)
cpuf=$(grep "physical id" /proc/cpuinfo | wc -l)
cpuv=$(grep "processor" /proc/cpuinfo | wc -l)
```

The wall command broadcasts the information to all logged-in users.

---

### Cron Job

The monitoring script runs automatically every 10 minutes using cron.

- Cron is a Linux service used for scheduling automated tasks.

- Crontab entry:

```*/10 * * * * /usr/local/bin/monitoring.sh```

This ensures the system status is regularly displayed without manual intervention.

# Bonus Part

The bonus section deploys a full web server environment.

Architecture:

```
Client
   │
   ▼
Lighttpd (Web Server)
   │
   ▼
PHP (Application Layer)
   │
   ▼
WordPress
   │
   ▼
MariaDB (Database)
```

---

### Lighttpd

```Lighttpd``` is a lightweight web server optimized for speed and low memory usage.

- (Its name is a portmanteau of "light" and "httpd"..!)

Advantages:

- High performance

- Low resource consumption

- Suitable for small servers

- Example command:

```sudo apt install lighttpd```

---

### MariaDB

MariaDB is an open-source relational database management system.

- It is a drop-in replacement for MySQL.

Advantages:

- High performance

- Strong community support

- Reliable data storage

- Example command:

```sudo apt install mariadb-server```

---

### PHP

PHP is a server-side scripting language widely used for web applications.

- It allows the web server to execute dynamic code.

- Example command:

```sudo apt install php php-fpm php-mysql```

---

### WordPress

WordPress is a popular content management system (CMS).

- It allows users to create websites easily without writing backend code.

- Features:

	- Plugin ecosystem

	- Theme system

	- User management

	- Content publishing

	- WordPress interacts with:

	- PHP (application layer)

	- MariaDB (database)

---

### Network Information

To retrieve network information:

```shell
hostname -I
ip link
```

This provides:

- Server IP address

- MAC address

# Resources

## Resources used:

Debian Documentation
https://www.debian.org/doc/

Linux Manual Pages
https://man7.org/linux/man-pages/

Born2beRoot Guide
https://noreply.gitbook.io/born2beroot

UFW Documentation
https://help.ubuntu.com/community/UFW

## Usage of A.I.

- Explain till I understand, and (I try my best to) internalise.

- Asks and learns "more than required" with A.I.

- Help with README formatting and better layout and paraphrasing of concepts.

---

### Conclusion

This project introduces the core responsibilities of a system administrator:

- installing and configuring a Linux server

- securing remote access

- managing users and permissions

- implementing firewall rules

- automating system monitoring

- Through this exercise, I have gained a practical understanding of how real-world servers are deployed and secured.

## Contact Details

#### For further queries/ help:

I am Christopher Hui-Kang Nah and my 42 intra is ```hnah```.

I am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42BeyondTheCode
```

##### EOF :D

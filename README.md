# LogoffUser
A simple command-line utilty for Windows that will:

* Log off user sessions on the current computer
* Switch to user sessions

Works on Windows 7, Vista and XP. The command will need to run with elevated permissions.

## Usage
Log off a single user called Lily:
    sessionutil logoff Lily
Log off users Lily and Tom:
    sessionutil logoff Lily Tom
Log off current user:
    sessionutil logoff
Switch to user Wendy:
    sessionutil switch Wendy
Go to the switch user screen:
    sessionutil switch



# "metered"
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('chillispot', 'ClearText-Password', ':=', 'thechillispotsecret');
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('antoinevg', 'ClearText-Password', ':=', 'antoinevg');

# "prepaid"
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('prepaid', 'ClearText-Password', ':=', 'prepaid');
# TODO - check why this is wanting it in radcheck!
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('prepaid', 'Max-Prepaid-Session', ':=', '3600');
# TODO - why does this not work ?
#INSERT INTO radreply (UserName, Attribute, op, Value) VALUES ('prepaid', 'Reply-Message', ':=', '%{User-Name} - %{Session-Timeout}');
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('bob', 'ClearText-Password', ':=', 'bob');
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('bob', 'Max-Prepaid-Session', ':=', '3600');
#INSERT INTO radreply (UserName, Attribute, op, Value) VALUES ('bob', 'Reply-Message', ':=', 'Hello, %u');

# "guest"
INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('guest', 'Password', ':=', 'guest');
INSERT INTO radreply (UserName, Attribute, op, Value) VALUES ('guest', 'Max-Daily-Session', ':=', '600');
INSERT INTO radreply (UserName, Attribute, op, Value) VALUES ('guest', 'Session-Timeout', ':=', '120');
INSERT INTO radreply (UserName, Attribute, op, Value) VALUES ('guest', 'Idle-Timeout', ':=', '60');



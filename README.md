mcabber-highlight
=================
An mcabber plugin to highlight specified messages.

Instructions
------------
Ensure you have mcabber development libraries installed
and that they're modern enough to use a pkg-config file.

Compile:

    $ make

Install:

    $ make install

Integrate:

    > /module load highlight
    > /highlight YoMamaWearsCombatBoots

Done.

Notes
-----
There is no explicit way to highlight a message within a
hook, so we set the attention flag instead. This would be
a lot easier if mcabber supplied the hook with a mutable
inbound XMPP message.

Fork it if you get any bright ideas.

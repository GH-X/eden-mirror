# Multiplayer for Local Co-Op Games

Use this guide when you want to play with a friend on a different system for games that only support local co-op.

**Click [Here](https://evilperson1337.notion.site/Multiplayer-for-Local-Co-Op-Games-2c657c2edaf680c59975ec6b52022a2d) for a version of this guide with images & visual elements.**

---

Occasionally you will want to play a game with a friend on a game that does not support LDN multiplayer, and only offer local co-op (multiple controllers connected to a single console), such as with *New Super Mario Bros. U Deluxe.*  Emulation solutions have developed 2 primary methods for handling these cases.

1. Netplay: Netplay lets two or more players run the same game on their own computers while sharing each other's controller inputs over the internet, so everyone sees the same game world in sync. One player hosts the session, and the others join as guests, sending their button presses back and forth to keep the gameplay coordinated.
    1. This is a huge over-simplification of how it works, but gives you an idea 
2. Low-Latency remote desktop solutions: Using a service like *Parsec*, the host shares his screen to a remote party with an input device connected.   This device sends inputs to the host machine.

In either situation at its core, we are emulating an input device on the host machine, so the game believes 2 controllers are connected.  No current Switch emulator has a Netplay offering, so we use Parsec to accomplish this for us.

---


### Pre-Requisites

- Eden Set Up and Fully Configured
- A [*Parsec*](https://parsec.app/) Account
    - Parsec is free to use for personal, non-commercial use.  For instructions on how to set up an account and install the client you should refer to the Parsec documentation on it’s site.
- Parsec client installed on your machine and remote (friend’s) machine

---

## Steps

<aside>

This guide will assume you are the one hosting the game and go over things *Parsec* specific at a high level, as their system is subject to change.  Follow *Parsec’s* documentation where needed.

</aside>

---

1. Launch Parsec on the host machine.
2. Connect to the other player in Parsec.  You will know it is successful when the other player can see the host’s screen.
    1. If you are the one hosting the game, you will have your friend initiate the remote connection you will accept.
    2. If you are joining a game, you will have to send a connection request the host will have to accept.
3. Verify that the remote player can see the screen and that there is no issues with the connection.
4. Launch Eden.
5. Navigate to *Emulation → Configure*.
6. Select the **Controls** tab.
7. Set up your controller, if necessary.
8. Select the **Player 2** tab and select the **Connect Controller** checkbox.  This enables inputs from another device to be seen as a second controller.
9. Dropdown the **Input Device** and select the controller.
    1. What exactly it shows up as depends on the Parsec settings.
10. Set up the remote player’s controller.
11. Hit **OK** to apply the changes.
12. Launch the game you want to play and enter the co-op mode.  How this works depends on the game, so you will have to look in the menus or online to find out.
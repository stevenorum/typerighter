## Structure

Just what's needed for the basic first version.

* Need a KeyPressed function to receive keypresses.
 * Updates text buffer and cursor as appropriate and then calls PrintScreen.
* Need a PrintScreen function that takes in the text buffer and the cursor location and prints the screen contents.
* Need a SaveBuffer function that stores the text buffer to persistent storage.
* Need a LoadBuffer function that loads the text buffer from persistent storage.

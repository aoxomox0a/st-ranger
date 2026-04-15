# sutta-ranger


## Suggestions for a good README

Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

## Name
Sutta Ranger TUI style reader

## Description
Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.


## Visuals
Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

## Installation
Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

## Usage
Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Support
Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

## Roadmap
# Sutta Ranger - Feature Roadmap & Architecture Log

### 1. Curated Sutta Roadmaps (Guided Study Paths)
**Concept:** A "Guide Mode" that offers a linear, curated reading experience as an alternative to the standard "Explorer Mode" (Miller Columns). Solves the problem of users feeling overwhelmed by the size of the Tipiṭaka.
* **Implementation:** Create a local `data/roadmaps.json` file acting as a "playlist" of Sutta IDs.
* **Data Structure:** Maps a human-readable topic (e.g., "Beginner's Path") to an array of IDs (e.g., `["mn61", "mn58", "an3.65"]`).
* **UI Flow:** When a user finishes a sutta in Guide Mode, a "Next Sutta" action automatically loads the next text in the sequence.

### 2. Persistent Reading Tracker
**Concept:** Allow users to track their progress through the Canon, bookmark texts, and visually distinguish between read and unread suttas.
* **Implementation:** Use `nlohmann/json` to write to a local `data/user_progress.json` file (ignored by Git) whenever the app closes or a state changes.
* **Data Structure:** Stores arrays for `"read_suttas"`, `"bookmarked"`, and a string for `"last_read"`.
* **UI Integration:** FTXUI conditionally styles the text in the explorer tree. E.g., `[✓]` prefix for read suttas, dim grey text for completed items, and bright colors for unread/bookmarked items.

### 3. Dashboard Gamification & Quick Resume
**Concept:** Combine the Roadmap and Tracker features to create a motivating user dashboard and reduce friction when reopening the app.
* **Implementation (Progress Bars):** Use FTXUI's `gauge` component on the Roadmap selection menu to calculate and display completion percentages (e.g., `[████░░░░░░] 40% (2/5 Read)`).
* **Implementation (Quick Resume):** Bind a hotkey (e.g., `r`) on the main menu that reads the `"last_read"` value from the progress JSON and instantly jumps the user back to the exact text they were studying in their previous session.

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Show your appreciation to those who have contributed to the project.

## License
For open source projects, say how it is licensed.

## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.

# Pixel Match Switcher

**Pixel Match Switcher** is an [OBS Studio](https://obsproject.com/) plugin that can trigger switching scenes, changing visibility of scene items and filters, and other actions in response to pixels of video being matched against image templates.
- Considerable amount of options is available for customizing the matching rules and the switching behavior.
- Significant effort has been made towards a quick and user-friendly creation of match rules.
- An individual matching image triggers transition to a specific *scene*, and several match entries (each with their own scene target) can be arranged in a priority-ordered list.
- Other actions can be triggered independently of each other:
    - scene item on/off
    - filter on/off
    - audio source mute/unmute
    - simulated hotkeys (must be assigned)
    - write to file (append or truncate)
    - frontend actions
        - streaming start/stop/pause
        - reacording start/stop/pause/unpause
        - replay buffer start/save/stop
        - take screenshot
        - virtual cam start/stop

## Foreseeable Use Cases
- Streamers often like to have displays with a bigger streamer-cam and/or ads when they are in a game menu, or some other “non gameplay” area of the game.
- Streamers often need to *hide* certain elements of game content, so they don’t get stream-sniped by people who want to gain an advantage by watching their stream while playing against them. Currently, streamers handle these cases manually, by manually switching scenes everytime, either with the OBS UI or by assigning hotkeys.
- Tournament broadcasters may wish to provide video overlays to identify participants and their team, or to show the game score. These could be activated or hidden automatically, in reaction to specific in-game graphics.
- Generally speaking, any use case where a region of a video frame will contain predictable pixels that should trigger one of the supported actions

## Requires Features Implemented in the OBS Fork

Presently, the plugin requires a special [atomic-effects](#Atomic-Effects-Fork-of-OBS) fork of OBS to function. We provide installers of the forked OBS with the plugin included to make everything easier to try out.

## Screenshot
![image](https://raw.githubusercontent.com/wiki/HoneyHazard/PixelMatchSwitcher/images/readme_screenshot.png)

## User Resources
- [Alpha Build Installers of OBS Fork + Plugin](https://github.com/HoneyHazard/PixelMatchSwitcher/releases)
- [User Manual](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual)
  - [Getting Started](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#getting-started)
  - [Pixel Match Filter](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#pixel-match-filter)
  - [UI Elements Overview](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#ui-elements-overview)
  - [Basic Concepts](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#basic-concepts)
  - [Image Capture Walkthrough](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#image-capture-walkthrough)
  - [UI Details](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/User-Manual#ui-details)
- [Discord](https://discord.gg/AeUavJpwQg)

## Atomic Effects Fork of OBS
Pixel Match Switcher plugin requires **effect results** and **atomic counters** to work. These precursor features are not present in the mainline of OBS Studio at the moment of writing. We provide the [atomic-effects](https://github.com/HoneyHazard/obs-studio-atomic-effects) fork to introduce these key features into OBS and make our plugin possible. Effort is being made to keep the fork up-to-date with the latest OBS commits.

[Changes](https://github.com/HoneyHazard/obs-studio-atomic-effects/wiki/Overview-of-the-changes-introduced-in-the-atomic-effects-fork-of-obs-studio) of the atomic-effects fork have been documented, but merging into the official OBS mainline is extremely unlikely. Developers' response was that the functionality of the fork is too specific to the plugin, and does not justify maintaining atomic effects changes in the official OBS. We are very open to folks including our fork's changes and the plugin in their own forks and builds, but please contact us so that proper credits are given if you plan on wide distribution. There are no plans to discontinue maintaining the fork, but not every update will be timely.

## Thought-About Expansion Features
- Advanced matching logic: AND, OR, etc
- Try to match an image anywhere in the video frame (not just at a fixed position) and allow show/hide of an image/mask source at the location where the match image would be found
- More advanced image processing could be made available for identifying elements in the video, so long as it is fast (and should probably stick to being implemented in the shader). This could broaden possible uses of the plugin.

## :construction: Build Resources
- [Windows: standalone plugin](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/Build-on-Windows%EA%9E%89-Standalone-Plugin)
- \[**TODO**\] Windows: OBS In-tree
- [Linux: standalone plugin](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/Build-on-Linux%EA%9E%89-Standalone-Plugin)
- [Debian-based Linux: OBS In-tree](https://github.com/HoneyHazard/PixelMatchSwitcher/wiki/Build-on-Debian-based-Linux%EA%9E%89-OBS-In-Tree)
- [obs-studio-pixel-match-switcher](https://github.com/PixelMatchSwitcher/obs-studio-pixel-match-switcher) is forked from [obs-studio-atomic-effects](https://github.com/HoneyHazard/obs-studio-atomic-effects), for the sole purpose of easy, in-tree builds of the plugin. These builds become [installer releases](https://github.com/HoneyHazard/PixelMatchSwitcher/releases) of Pixel Match Switcher.

## Special Thank You
:thumbsup: Special Thank You goes out to [Exceldro](https://github.com/exeldro) for letting [obs-move-transition](https://github.com/exeldro/obs-move-transition) plugin be included with our installer builds.

## Related Github Projects
- [obs-studio-atomic-effects](https://github.com/HoneyHazard/obs-studio-atomic-effects)
- [SceneSwitcher](https://github.com/WarmUpTill/SceneSwitcher)
- [obs-screenshot-plugin](https://github.com/synap5e/obs-screenshot-plugin)
- [obs-move-transition](https://github.com/exeldro/obs-move-transition)

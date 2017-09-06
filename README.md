# Small N Stats Data Tracker (Qt)

Data Tracker is a Qt-based application that assists researchers and clinicians in reliably measuring behavior of interest. This program can be used by multiple observers to record, and compare, behavior in real-time. Additionally, this tool provides multiples methods for assessing the reliability of measurements, session-by-session viewing and interpretation, and the automation of clinical responsibilities.

Features include:
  - Cross-platform support for Windows, Mac, and Linux (only Ubuntu tested)
  - Customizable methods for designing measurement systems
  - Real-time recording of behavior with multiple observers
  - Tools for calculation of reliability indices
  - Optionally automate reliability measures and reports as new data is added
  - Automated update delivery, for keeping up to date with new features and bug fixes
  - Options for saving behavioral data in multiple locations and formats (JSON, xlsx, etc.)
  - Saving figures and reports in a range of common formats

### Version
------
0.6.0 (beta)

### Changelog
------
 * 0.6.0 - Time window-based lag sequential analyses (and adjustable window size) for sessions
 * 0.5.2 - Missing dependency
 * 0.5.1 - Compatability fix
 * 0.5.0 - Bump with latest for initial public beta
 * 0.4.1 - Interface cleanup, for cross-platform compatibility
 * 0.4.0 - Differentiate series-based charting, more mac os fixes
 * 0.3.0 - Add in new charting
 * 0.2.0 - File migration fixes, updates to auto-scoring, results dialog for reli
 * 0.1.0 - Auto update/notification
 * 0.0.9 - Session viewer
 * 0.0.8 - Automated file migrations (after every change, optional)
 * 0.0.7 - Automated reliability calculations (after every session, optional)
 * 0.0.6 - Theming (light and dark modes)
 * 0.0.5 - JSON and xlsx file outputs (optional)
 * 0.0.4 - Saveable options for file output
 * 0.0.3 - Post-session cumulative recording plots (optional)
 * 0.0.2 - Calculation of reliability
 * 0.0.1 - Barebones framework

### Features/Usage
------
Robust, customizable data collection options
![Alt text](screencaps/SessionDesigner.png?raw=true "Session Designer")

Real-time data recording for multiple observers
![Alt text](screencaps/RecordingWindow.png?raw=true "Recording Window")

Visualizations of behavior across conditions (e.g., baseline, treatment)
![Alt text](screencaps/EvaluationViewer.png?raw=true "Evaluation Viewer")

Session-by-session Sequental Analyses (e.g., Prompts -> Target Behavior statistics)
![Alt text](screencaps/SequentialAnalyses.png?raw=true "Sequential Analyses")

Within-session visualizations of responding during session
![Alt text](screencaps/SessionViewer.png?raw=true "Session Viewer")

Calculation of Reliability Indices
![Alt text](screencaps/Reliability.png?raw=true "Reliability Calculator")

Preparation of Reports
![Alt text](screencaps/Report.png?raw=true "Evaluation Reports")

### Referenced Works (F/OSS software)
------
 * Qt Framework (5.8) - LGPL 3.0 (GPL 3.0 Licensed). Copyright (C). Qt Team [Site](https://www.qt.io/)
 * Tango icon set - Public Domain Licensed. Copyright (C) 2009, "Tango Desktop Project" [Site](http://tango.freedesktop.org/Tango_Desktop_Project)
 * QtXlsx - MIT Licensed. Copyright (C) 2013, Debao Zhang <hello@debao.me> [Github](https://github.com/dbzhang800/QtXlsxWriter)
 * QDarkStyleSheet - MIT Licensed. Copyright (C) 2013-2017, Colin Duquesnoy <colin.duquesnoy@gmail.com> [Github](https://github.com/ColinDuquesnoy/QDarkStyleSheet)
 * BDataPro - GPL-v3. Copyright (C) 2017, Chris Bullock. [Site](https://www.kennedykrieger.org/patient-care/patient-care-programs/inpatient-programs/neurobehavioral-unit-nbu/bdatapro-software-for-real-time-behavior-data-collection).

### Referenced Works (academic works)
------
The Small N Stats Data Tracker is based on the following academic works:

 * Bullock, C. E., Fisher, W. W., & Hagopian, L. P. (2017). Description and Validation of a Computerized Behavioral Data Program: "BDataPro". The Behavior Analyst, 1-11. [doi:https://dx.doi.org/10.1007/s40614-016-0079-0](https://dx.doi.org/10.1007/s40614-016-0079-0)

### Acknowledgements and Credits
------
 * Donald A. Hantula, Decision Making Laboratory, Temple University [Site](http://astro.temple.edu/~hantula/)
 * Chris Bullock, Kennedy Krieger Institute (Site)[https://www.kennedykrieger.org/patient-care/patient-care-programs/inpatient-programs/neurobehavioral-unit-nbu/bdatapro-software-for-real-time-behavior-data-collection].

### Installation
------
No other packages are required. Simply build and run or install and run.

### Download
------
All downloadable binaries, if/when posted, will be hosted at [Small N Stats](http://www.smallnstats.com).

### Development
------
Want to contribute? Great! Emails or PM's are welcome.

### Todos
------
 - Post-session key editing (for fixing errors)
 - Statistical analyses (lag sequential analyses)
 - Treatment summaries and reports

### License
----
Data Tracker (Qt) - Copyright 2017, Shawn P. Gilroy. GPL-Version 3

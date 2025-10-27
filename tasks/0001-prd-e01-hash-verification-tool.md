# Product Requirements Document: E01 Hash Verification Tool

## Introduction/Overview

The E01 Hash Verification Tool is a Windows desktop application designed to provide a simple, high-performance GUI for verifying forensic image file hashes. The tool addresses the need for digital forensic analysts and law enforcement personnel to quickly verify the integrity of forensic disk images without relying on command-line tools. By leveraging the libewf library, the application will support E01, Ex01, DD, ZIP, and all other libewf-supported formats, with both monolithic and segmented file support.

**Problem Statement:** Current hash verification workflows for forensic images require command-line proficiency, which creates barriers for non-technical law enforcement personnel and slows down verification workflows. A GUI tool that matches command-line performance while providing intuitive visual feedback will streamline forensic investigations.

**Goal:** Create a production-ready Windows desktop application that makes forensic image hash verification accessible to both technical and non-technical users while maintaining the performance of libewf command-line tools.

## Goals

1. Deliver hash verification performance that matches or exceeds libewf command-line tools
2. Provide an intuitive single-window interface that requires no documentation or training
3. Support all libewf-compatible file formats (E01, Ex01, DD, ZIP, segmented files)
4. Achieve zero crashes during normal operation
5. Distribute a complete MSI installer with all dependencies bundled
6. Release as free, open-source software under permissive licensing (MIT or Apache 2.0)

## User Stories

**US-001:** As a digital forensic analyst, I want to drag-and-drop an E01 file onto the application so that I can quickly verify its integrity without typing commands.

**US-002:** As a law enforcement officer with limited technical background, I want to see clear visual indicators (green checkmark or red X) so that I immediately understand whether the evidence file is verified without reading technical documentation.

**US-003:** As a forensic examiner, I want the tool to automatically detect and select hash algorithms encoded in the E01 metadata so that I don't have to manually determine which hashes to verify.

**US-004:** As a user processing large forensic images, I want to see real-time progress with estimated time remaining so that I can plan my workflow and know when verification will complete.

**US-005:** As a forensic analyst, I want the ability to cancel a long-running verification operation so that I can correct mistakes or reprioritize work without waiting for completion.

**US-006:** As a non-technical user, I want clear error messages with actionable guidance (like "Run as Administrator") so that I can resolve problems without technical support.

**US-007:** As a forensic examiner, I want to optionally calculate additional hash types beyond what's encoded in the file so that I can meet varying evidence requirements from different jurisdictions.

## Functional Requirements

### FR-1: File Input
1.1. The application SHALL support drag-and-drop for single file input only
1.2. The application SHALL provide a "Browse" button for traditional file selection
1.3. The application SHALL accept E01, Ex01, DD, ZIP, and all libewf-supported formats
1.4. The application SHALL automatically detect segmented file structures by reading E01/Ex01 file metadata and load all segments
1.5. Drag-and-drop SHALL NOT accept folder input; only individual files
1.6. When a new file is loaded during active verification, the application SHALL automatically replace the current operation with the new file

### FR-2: Metadata Display
2.1. The application SHALL extract and display encoded metadata from E01/Ex01 files
2.2. The application SHALL display encoded hash values (MD5, SHA1, SHA256 if present)
2.3. Metadata display SHALL appear in the top section of the window
2.4. For files without encoded metadata (e.g., raw DD images), the application SHALL display "No encoded metadata available"

### FR-3: Hash Algorithm Selection
3.1. The application SHALL provide checkboxes for MD5, SHA1, and SHA256 hash algorithms
3.2. The application SHALL automatically check/select hash types that are encoded in the source file
3.3. Users SHALL be able to manually check/uncheck hash algorithms before verification begins
3.4. Users SHALL be able to select additional hash algorithms beyond those encoded in the file
3.5. At least one hash algorithm MUST be selected to enable verification

### FR-4: Hash Verification
4.1. The application SHALL calculate selected hashes using libewf library functions
4.2. Hash calculation performance SHALL match or exceed libewf command-line tool performance
4.3. The application SHALL support files of any size without artificial limitations
4.4. The application SHALL compare calculated hashes against encoded values (when available)
4.5. Verification results SHALL display in the bottom section of the window
4.6. For each verified hash match, the application SHALL display a green checkmark icon with "Verified" text
4.7. For each hash mismatch, the application SHALL display a red X icon with "Not Verified" text
4.8. For hashes without encoded values, the application SHALL display the calculated hash value only

### FR-5: Progress Indication
5.1. The application SHALL display a progress bar showing percentage complete (0-100%)
5.2. The application SHALL display estimated time remaining during verification
5.3. Progress indicators SHALL update in real-time (minimum 1 update per second)
5.4. The application SHALL maintain UI responsiveness during hash calculation
5.5. When calculating multiple hashes simultaneously, progress reporting SHALL be based on SHA1 and MD5 algorithms

### FR-6: Verification Control
6.1. The application SHALL provide a "Cancel" button during active verification
6.2. When Cancel is clicked, the application SHALL display a confirmation dialog: "Are you sure you want to cancel verification?"
6.3. Upon confirmation, the application SHALL halt verification and discard all partial results
6.4. The application SHALL return to ready state after cancellation

### FR-7: Error Handling
7.1. When a file segment is missing, the application SHALL display: "Missing segment. File e## is missing" (with specific segment number)
7.2. When a corrupted file is detected, the application SHALL display: "Corrupted File Encountered. Cannot perform hash verification."
7.3. When file access is denied due to permissions, the application SHALL display: "You have insufficient permission to hash this file. Try running the program as Administrator."
7.4. When errors occur, the application SHALL discard all partial results (no partial hash values displayed)
7.5. Error messages SHALL be displayed in a prominent dialog box with an OK button

### FR-8: Logging (Optional Feature)
8.1. The application MAY provide optional logging functionality
8.2. Logging SHALL be disabled by default (opt-in via settings)
8.3. If implemented, logs SHALL include: file paths, verification results, timestamps, and error messages
8.4. If implemented, log files SHALL be stored in a user-accessible location (e.g., `%APPDATA%\E01HashTool\logs\`)
8.5. Logging SHALL NOT impact verification performance

### FR-9: Single File Processing
9.1. The application SHALL process one file or file set at a time
9.2. The application SHALL NOT implement batch queuing functionality
9.3. Starting a new verification SHALL automatically replace the previous verification session

## Non-Goals (Out of Scope)

1. **Batch Processing:** The application will NOT support queuing multiple files for sequential verification
2. **Partial Segment Verification:** The application will NOT verify individual segments independently (complete verification only)
3. **Hash Encoding/Writing:** The application will NOT write or modify hash values in forensic images (read-only verification only)
4. **Image Mounting:** The application will NOT mount or browse the contents of forensic images
5. **Cross-Platform Support:** Initial release will NOT support macOS or Linux (Windows 10/11 only)
6. **Image Creation:** The application will NOT create forensic images (verification only)
7. **Detailed Forensic Analysis:** The application will NOT provide file system analysis, timeline generation, or other forensic examination features
8. **Network/Cloud Storage:** The application will NOT directly access network shares or cloud storage (local file access only)
9. **File Associations:** The installer will NOT register file associations for .E01 files or modify existing associations
10. **Auto-Update Mechanism:** The application will NOT include automatic update functionality (manual updates only)
11. **Folder Drag-and-Drop:** The application will NOT support dragging entire folders; single file drag-and-drop only

## Design Considerations

### UI Mockups

#### Mockup 1: Initial State (Ready for Input)
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                                                       │    │
│  │          Drop forensic image file here               │    │
│  │                      or                              │    │
│  │                 [ Browse... ]                        │    │
│  │                                                       │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                               │
│  Hash Algorithms to Verify:                                  │
│    ☐ MD5     ☐ SHA1     ☐ SHA256                            │
│                                                               │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

#### Mockup 2: File Loaded - Showing Encoded Metadata
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│  File: evidence_2024_case_001.E01                            │
│                                                               │
│  ┌─ Encoded Metadata ──────────────────────────────────┐    │
│  │  Case Number:   2024-001                             │    │
│  │  Evidence ID:   HD-001                               │    │
│  │  Description:   Suspect laptop hard drive            │    │
│  │  Examiner:      J. Smith                             │    │
│  │                                                       │    │
│  │  Encoded Hashes:                                     │    │
│  │    MD5:    d41d8cd98f00b204e9800998ecf8427e          │    │
│  │    SHA1:   da39a3ee5e6b4b0d3255bfef95601890afd80709  │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
│  Hash Algorithms to Verify:                                  │
│    ☑ MD5     ☑ SHA1     ☐ SHA256                            │
│                                                               │
│                         [ Start Verification ]               │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

#### Mockup 3: Verification in Progress
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│  File: evidence_2024_case_001.E01                            │
│                                                               │
│  ┌─ Encoded Metadata ──────────────────────────────────┐    │
│  │  Case Number:   2024-001                             │    │
│  │  Evidence ID:   HD-001                               │    │
│  │  Description:   Suspect laptop hard drive            │    │
│  │  Examiner:      J. Smith                             │    │
│  │                                                       │    │
│  │  Encoded Hashes:                                     │    │
│  │    MD5:    d41d8cd98f00b204e9800998ecf8427e          │    │
│  │    SHA1:   da39a3ee5e6b4b0d3255bfef95601890afd80709  │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
│  ┌─ Verification Progress ─────────────────────────────┐    │
│  │                                                       │    │
│  │   [████████████████░░░░░░░░░░░░░░] 67%              │    │
│  │                                                       │    │
│  │   Estimated time remaining: 2 minutes 34 seconds     │    │
│  │                                                       │    │
│  │                      [ Cancel ]                       │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

#### Mockup 4: Verification Success (All Hashes Match)
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│  File: evidence_2024_case_001.E01                            │
│                                                               │
│  ┌─ Encoded Metadata ──────────────────────────────────┐    │
│  │  Case Number:   2024-001                             │    │
│  │  Evidence ID:   HD-001                               │    │
│  │  Description:   Suspect laptop hard drive            │    │
│  │  Examiner:      J. Smith                             │    │
│  │                                                       │    │
│  │  Encoded Hashes:                                     │    │
│  │    MD5:    d41d8cd98f00b204e9800998ecf8427e          │    │
│  │    SHA1:   da39a3ee5e6b4b0d3255bfef95601890afd80709  │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
│  ┌─ Verification Results ──────────────────────────────┐    │
│  │                                                       │    │
│  │   ✓  MD5:    Verified                                │    │
│  │      d41d8cd98f00b204e9800998ecf8427e                │    │
│  │                                                       │    │
│  │   ✓  SHA1:   Verified                                │    │
│  │      da39a3ee5e6b4b0d3255bfef95601890afd80709        │    │
│  │                                                       │    │
│  │   Verification completed in 4 minutes 12 seconds     │    │
│  │                                                       │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

#### Mockup 5: Verification Failure (Hash Mismatch)
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│  File: evidence_2024_case_001.E01                            │
│                                                               │
│  ┌─ Encoded Metadata ──────────────────────────────────┐    │
│  │  Case Number:   2024-001                             │    │
│  │  Evidence ID:   HD-001                               │    │
│  │  Description:   Suspect laptop hard drive            │    │
│  │  Examiner:      J. Smith                             │    │
│  │                                                       │    │
│  │  Encoded Hashes:                                     │    │
│  │    MD5:    d41d8cd98f00b204e9800998ecf8427e          │    │
│  │    SHA1:   da39a3ee5e6b4b0d3255bfef95601890afd80709  │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
│  ┌─ Verification Results ──────────────────────────────┐    │
│  │                                                       │    │
│  │   ✗  MD5:    Not Verified                            │    │
│  │      Expected: d41d8cd98f00b204e9800998ecf8427e      │    │
│  │      Calculated: 8f7e8a9c3b2d1e4f5a6b7c8d9e0f1a2b    │    │
│  │                                                       │    │
│  │   ✓  SHA1:   Verified                                │    │
│  │      da39a3ee5e6b4b0d3255bfef95601890afd80709        │    │
│  │                                                       │    │
│  │   WARNING: Hash verification failed!                 │    │
│  │   This file may be corrupted or modified.            │    │
│  │                                                       │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

#### Mockup 6: Error State (Corrupted File)
```
┌─────────────────────────────────────────────────────────────┐
│  E01 Hash Verification Tool                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│                                                               │
│     ┌─────────────────────────────────────────────────┐     │
│     │              ⚠  Error                           │     │
│     │                                                  │     │
│     │  Corrupted File Encountered.                    │     │
│     │  Cannot perform hash verification.              │     │
│     │                                                  │     │
│     │                   [ OK ]                         │     │
│     └─────────────────────────────────────────────────┘     │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

### UI Design Principles
- **Simplicity First:** Single window, no tabs or complex navigation
- **Visual Hierarchy:** Metadata at top, progress in middle, results at bottom
- **Color Coding:** Green for success (verified), red for failure (not verified), yellow/orange for warnings
- **Responsive Feedback:** All actions provide immediate visual feedback
- **Minimal Chrome:** Clean interface without unnecessary decorations or branding
- **Accessibility:** Font size minimum 10pt, high contrast for color-blind users

### Technical UI Framework
- **Recommended:** Qt 5.x or Qt 6.x for cross-platform GUI framework (compiles to native Windows)
- **Alternative:** Windows Forms or WPF if pure Windows-native preferred
- **Rationale:** Qt provides professional controls, excellent threading support, and native look-and-feel on Windows

## Technical Considerations

### Architecture
- **Language:** C++ (for libewf integration and performance)
- **GUI Framework:** Qt (recommended) or native Windows frameworks
- **Threading Model:**
  - GUI thread for UI updates and user interaction
  - Worker thread(s) for hash calculation if implementation time < 2 weeks
  - Single-threaded hash calculation acceptable if multi-threading adds > 2 weeks development time
  - **Decision:** Implement single-threaded first; defer multi-threading to Phase 2 if performance is acceptable
- **Library Integration:** Static linking preferred for all dependencies (libewf, Qt, OpenSSL)
- **Rationale:** Static linking produces larger executables but simplifies deployment and ensures "it just works" for end users

### Performance Requirements
- Hash calculation speed MUST match or exceed libewf CLI tools
- UI MUST remain responsive during verification (no freezing)
- Memory usage should be efficient (streaming hash calculation, not loading entire file into RAM)
- Progress updates MUST occur at least once per second

### Platform Requirements
- **Target OS:** Windows 10 (1809 or later) and Windows 11
- **Architecture:** 64-bit (x86_64) primary, 32-bit optional
- **Dependencies:** All runtime dependencies MUST be statically linked into the executable
- **Installation:** Single MSI installer that requires no external dependencies or chained installers
- **Installer Size:** Larger installer size is acceptable to achieve zero-dependency deployment

### Security Considerations
- Application MUST NOT modify source files (read-only access)
- Application should request elevation (Run as Administrator) only if file permissions require it
- Log files (if implemented) MUST NOT contain sensitive case information unless user explicitly enables detailed logging

### Licensing
- **Recommended License:** MIT or Apache 2.0 for maximum compatibility
- **Important Note:** libewf uses LGPL. Static linking with LGPL libraries may require providing object files for relinking (LGPL compliance). Alternative: Consider using dynamic linking for libewf only while statically linking other dependencies.
- **Compliance Requirement:** If statically linking LGPL libraries, must provide mechanisms for users to relink with modified versions (per LGPL §6)
- **Distribution:** Free and open-source, no commercial sale

### Known Dependencies (Statically Linked)
- **libewf:** Core library for E01/Ex01 format support (static)
- **OpenSSL or Windows CryptoAPI:** For hash calculation (MD5, SHA1, SHA256) - statically linked if OpenSSL chosen
- **Qt libraries:** If Qt framework chosen (static Qt build required)
- **Note:** All dependencies will be statically linked to create a single, standalone executable with no external DLL requirements

## Success Metrics

The E01 Hash Verification Tool will be considered successful when it meets the following criteria:

1. **Performance Parity:** Hash verification speed matches or exceeds libewf command-line tools (measured on same hardware with same test files)

2. **Usability - Zero Documentation Required:** Non-technical users can successfully verify forensic image hashes without consulting documentation, tutorials, or technical support

3. **Reliability - Zero Crashes:** Application achieves zero crashes during normal operation across 100+ test verification sessions on various file types and sizes

4. **User Satisfaction (Post-Launch):** Positive feedback from forensic analyst and law enforcement user communities

5. **Installation Success:** MSI installer successfully deploys on clean Windows 10/11 systems without manual dependency installation

## Design Decisions - Resolved

The following design decisions have been finalized based on stakeholder input:

1. **Multi-threading Implementation:**
   - **Decision:** Implement single-threaded hash calculation in Phase 1. Defer multi-threading to Phase 2 only if single-threaded performance does not match libewf CLI tools.
   - **Rationale:** Simplify initial development; multi-threading adds complexity that may not be necessary

2. **Hash Algorithm Priority for Progress Reporting:**
   - **Decision:** When calculating multiple hashes simultaneously, progress reporting will be based on SHA1 and MD5 algorithms
   - **Rationale:** These are the most commonly encoded hash types in forensic images

3. **Installer Dependencies:**
   - **Decision:** Static linking for all dependencies (larger installer, zero runtime dependencies)
   - **Rationale:** Prioritize "it just works" deployment experience over installer size. No external DLLs or chained installers required.

4. **Logging Default State:**
   - **Decision:** Logging disabled by default (opt-in via settings if implemented)
   - **Rationale:** Avoid privacy concerns with case-sensitive information; users can enable if needed for debugging

5. **Drag-and-Drop Scope:**
   - **Decision:** Single file drag-and-drop only (no folder support)
   - **Rationale:** Segmented files are auto-detected by reading E01/Ex01 file metadata, not by folder structure. Keeps implementation simple.

6. **Update Mechanism:**
   - **Decision:** Manual updates only (no auto-update functionality)
   - **Rationale:** Keep application simple; auto-update adds significant complexity for minimal benefit

7. **File Association:**
   - **Decision:** Do NOT register file associations for .E01 files
   - **Rationale:** Avoid interfering with existing forensic tool workflows; users can manually open files via drag-and-drop or browse

8. **Progress Granularity:**
   - **Decision:** Minimum 1 update per second; evaluate sub-second updates during development with large test files (1TB+)
   - **Rationale:** Balance UX responsiveness with implementation complexity; test with real-world large files

## Appendix: User Testing Checklist

Before release, the application should be tested by at least 3 users representing the target audience:
- [ ] 1 experienced digital forensic analyst (technical user)
- [ ] 2 law enforcement personnel with varying technical backgrounds (non-technical to moderately technical)

Testing scenarios:
- [ ] Verify a monolithic E01 file with encoded MD5 and SHA1
- [ ] Verify a segmented E01 file set (E01, E02, E03, etc.)
- [ ] Verify a raw DD image (no encoded hashes)
- [ ] Attempt to verify a corrupted file (test error handling)
- [ ] Cancel a verification in progress
- [ ] Verify multiple files in sequence (testing workflow)

Success criteria for user testing:
- All users successfully complete verification without asking questions
- All users correctly interpret verification results (pass/fail)
- No confusion about error messages or next steps

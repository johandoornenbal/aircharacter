# Third-party provenance

## Airwindows character engines

Source: https://github.com/airwindows/airwindows
Commit: `94cb6793893027fb418ffac7cdbfcb9b4c82aadb` (retrieved 2026-11-14).
Original `plugins/WinVST/` headers, wrappers and processing sources for
PurestWarm3, Discontapeity, Sweeten, Inflamer, Coils2, Density3, Weave, Longhand,
PurestSaturation, Hypersoft, Creature and Tube2 are retained
unchanged under `third_party/airwindows`, with the upstream MIT license.
`SHA256.json` records each source file's hash. Copyright Chris Johnson / Airwindows.

Standalone `Source/*Engine.*` files remove VST2 metadata and expose reset and
normalized setters. Both processing bodies remain byte-identical. Discontapeity
retains a private VstInt32 alias for its original local declaration. Constructor
initialization is reused by reset, with deterministic `0x12345678` / `0x87654321`
noise seeds. This changes the noise sequence relative to a randomly seeded
original instance, as in the existing wrappers, without changing processing.
Setters clamp normalized controls to 0–1. Reference tests compile untouched
upstream code with the test-only, independently written minimal VST2 interface;
no VST2 SDK is used. Matching initial noise seeds permits exact sample tests.

No fixed lookahead is added. Original signal-dependent delay/filter behavior is
retained, including Discontapeity's variable delay. Input/output trims, LUFS gain,
A/B crossfade and output metering are separate wrapper features. At unity trims
without an active learned match, Processed returns the original engine output.

## Wrapper and UI

Wrapper, meters, LUFS matcher, gain linking, test patterns and Mac script adapted
from AirEq Studio, https://github.com/johandoornenbal/AirEq at
`78c3719851693b4f6b9fdd59f2ca587c33ab3f11` (MIT), using the same conventions as
https://github.com/johandoornenbal/tape. Prior contributors' notices remain in
LICENSE. The lineage includes ToTape Studio and ButterComp Studio
(https://github.com/johandoornenbal/codex-tryout); the ButterComp notice is
retained in `third_party/ButterComp-LICENSE`.

## Loudness reference

Stereo BS.1770 K-weighting filter design follows libebur128 by Jan Kokemueller
(MIT), pinned at `67b33abe1558160ed76ada1322329b0e9e058b02` from
https://github.com/jiixyj/libebur128.
The original C source and license are retained under `third_party/libebur128`
as an independent test oracle, not linked into the plugin. The queue compatibility
code retains its embedded BSD notices. `Source/LoudnessMatch.h` uses bounded
400 ms windows, 100 ms hops, a −70 LUFS absolute gate and −10 LU relative gate.

## JUCE and VST3

JUCE 8.0.12: `29396c22c93392d6738e021b83196283d6e4d850`.
https://github.com/juce-framework/JUCE/blob/8.0.12/LICENSE.md

JUCE is dual-licensed under AGPLv3 and commercial terms. Distribution must comply
with the applicable JUCE license, including corresponding source obligations when
using AGPLv3, or use an appropriate commercial license. The wrapper's MIT license
does not relicense JUCE. Retain the VST3 SDK notices bundled with JUCE.
This is an independent adaptation, not an official Airwindows or JUCE release.

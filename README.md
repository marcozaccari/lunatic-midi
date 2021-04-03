# Lunatic Midi

## Goal

- Modular: configure and combine components and modules to obtain a custom keyboard;
- Efficient: low latency firmwares;
- Simple and cheap: poor man design, with minimal electronic components and easy assembly.

## System diagram

```plantuml
@startuml
    skinparam componentStyle rectangle
    skinparam roundCorner 15

    interface MIDI as MIDI #plum;line:blueviolet

    package " Controller board  " #seashell {
        [Controller] #white
        [Driver] #white

        MIDI -[#blueviolet;thickness=4] [Controller]
        [Controller] - IPC
        IPC - [Driver]
    }

    interface "bus" as BUS #aliceblue;line:blue
    [Driver] -[#blue;thickness=4]- BUS

    package " Component(s) " {
        node "Logic module" as LMOD #honeydew 
        node "Interface module(s)" as IMOD #honeydew;line.dotted
        interface "PIO" as PIOM
    
        BUS -[#blue;thickness=4] LMOD
        LMOD -[thickness=4] PIOM
        PIOM -[thickness=4] IMOD
    }

    interface "PIO" as PIOH
    IMOD -[thickness=4]- PIOH

    node Hardware #aliceblue
    PIOH -[thickness=4] Hardware
@enduml
```

A single controller sends and receives events from a bus to which several components are connected.
Each component, consisting of a logic board and eventually one or more interface boards, communicates with the final hardware.

The final hardware can be mechanical keyboards, pedal boards, buttons and lights, analog controls such as knobs and sliders, etc.

In the controller, the driver coordinates the components using specific communications protocols, while a high-level software take cares of traslating events from-and-to MIDI messages.


## Example: Electronic Keyboard

```plantuml
@startuml
    skinparam componentStyle rectangle
    skinparam roundCorner 15

    interface MIDI as MIDI #plum;line:blueviolet

    [CONTROLLER] as "Controller" #seashell
    interface "bus" as BUS #aliceblue;line:blue

    [CONTROLLER] -[#blue] BUS

    MIDI -[#blueviolet] [CONTROLLER]

    [KCOMP] as "Keyboard" #honeydew
    [ACOMP] as "Analog Knobs" #honeydew
    [BCOMP] as "Buttons and lights" #honeydew

    BUS -[#blue]d- [BCOMP]
    BUS -[#blue]d- [KCOMP]
    BUS -[#blue]d- [ACOMP]

@enduml
```

## Example: Organ

```plantuml
@startuml
    skinparam componentStyle rectangle
    skinparam roundCorner 15

    interface MIDI as MIDI #plum;line:blueviolet

    [CONTROLLER] as "Controller" #seashell
    interface "bus" as BUS #aliceblue;line:blue

    [CONTROLLER] -[#blue]d- BUS

    MIDI -[#blueviolet] [CONTROLLER]

    [KCOMP1] as "Keyboard #1" #honeydew
    [KCOMP2] as "Keyboard #2" #honeydew
    [PCOMP] as "Pedals" #honeydew
    [BCOMP] as "Buttons and lights" #honeydew

    BUS -[#blue]d- [BCOMP]
    BUS -[#blue]d- [KCOMP1]
    BUS -[#blue]d- [KCOMP2]
    BUS -[#blue]d- [PCOMP]
@enduml
```

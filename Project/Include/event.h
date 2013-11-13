
/*
  Filename: event.h
  Description: This file contains the declarations for the event API.
*/

#ifndef __EVENT_H__
# define __EVENT_H__

// configuration defines
// if used, within the event APIs, the interrupts are locked
# define INTERRUPT_LOCK_EVENT_API

typedef uint16_t EventMaskType;

# define EVENT_NO_EVENT                 ((EventMaskType) 0x0000)

# define EVENT_COM_SLOT_START           ((EventMaskType) 0x0001)
# define EVENT_COM_SLOT_RX_START        ((EventMaskType) 0x0002)
# define EVENT_COM_SLOT_TX_START        ((EventMaskType) 0x0004)
# define EVENT_COM_SLOT_OPEN_SYNC_RX    ((EventMaskType) 0x0008)
# define EVENT_UNUSED0x0010             ((EventMaskType) 0x0010)
# define EVENT_UNUSED0x0020             ((EventMaskType) 0x0020)
# define EVENT_UNUSED0x0040             ((EventMaskType) 0x0040)
# define EVENT_UNUSED0x0080             ((EventMaskType) 0x0080)
# define EVENT_UNUSED0x0100             ((EventMaskType) 0x0100)
# define EVENT_UNUSED0x0200             ((EventMaskType) 0x0200)
# define EVENT_BUTTON_DOWN              ((EventMaskType) 0x0400)
# define EVENT_BUTTON_NUM               ((EventMaskType) 0x0800)
# define EVENT_BUTTON_STAR              ((EventMaskType) 0x1000)
# define EVENT_BUTTON_BACKLIGHT         ((EventMaskType) 0x2000)
# define EVENT_BUTTON_UP                ((EventMaskType) 0x4000)
# define EVENT_DISPLAY_TICK             ((EventMaskType) 0x8000)

// simple macro implementation, deadlocks if not used properly!
// has to be changed...
// or is it even not necessary?
//# define WaitEvent(eventmask) while((EventMaskType)0 == GetEvent(eventmask))

EventMaskType GetEvent(EventMaskType eventmask);
void ClearEvent(EventMaskType eventmask);
void SetEvent(EventMaskType eventmask);

#endif /* __EVENT_H__ */
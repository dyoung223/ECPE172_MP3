#ifndef _SOUND_H
#define _SOUND_H

void writeDACs( void );
void fillBuffers( uint32_t *left, uint32_t *right, uint32_t len );

void initSound( void );
bool isSoundFinished( void );

void upVolume( void );
void downVolume( void );

#endif // _SOUND_H

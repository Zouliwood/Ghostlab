#include "game.h"

game* get_game(listElements *games,uint8_t game_id)
{
    element *ptr = games->first;
    while (ptr)
    {
        if(((game *)ptr->data)->game_id==game_id){
            return ptr->data;
        }else{
            ptr=ptr->next;
        }
    }
    return NULL;
}
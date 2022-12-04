// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}

// Auto Attack
static int buildin_autoattack_sub(struct block_list *bl,va_list ap)
{
	int *target_id=va_arg(ap,int *);
	*target_id = bl->id;
	return 1;
}

void autoattack_motion(struct map_session_data* sd)
{
	int i, target_id;
	if( pc_isdead(sd) || !sd->state.autoattack || pc_ishiding(sd) || pc_cant_act(sd) ) return;

	for(i=0;i<=9;i++)
	{
		target_id=0;
		map_foreachinarea(buildin_autoattack_sub, sd->bl.m, sd->bl.x-i, sd->bl.y-i, sd->bl.x+i, sd->bl.y+i, BL_MOB, &target_id);
		if(target_id){
			unit_attack(&sd->bl,target_id,1);
			break;
		}
		target_id=0;
	}
	if(!target_id && !pc_isdead(sd) && sd->state.autoattack){
		unit_walktoxy(&sd->bl,sd->bl.x+(rand()%2==0?-1:1)*(rand()%25),sd->bl.y+(rand()%2==0?-1:1)*(rand()%25),0);
	}
	return;
}

static TIMER_FUNC(autoattack_timer)
{
	struct map_session_data *sd=NULL;

	sd=map_id2sd(id);
	if(sd==NULL || pc_isdead(sd) || !sd->state.autoattack )
		return 0;

	if(sd->state.autoattack)
	{
		unit_stop_attack(&sd->bl);
		autoattack_motion(sd);
		if(DIFF_TICK(sd->autoattack_delay,gettick())> 0){
			clif_authfail_fd(sd->fd, 15);
			return 0;
		}
		else{
			add_timer(gettick()+1000,autoattack_timer,sd->bl.id,0);
			sd->autoattack_delay = gettick() + 1000;
		}
	}
	return 0;
}

ACMD_FUNC(autoattack)
{
	nullpo_retr(-1, sd);
	if (sd->state.autoattack)
	{
		sd->state.autoattack = 0;
		unit_stop_attack(&sd->bl);
		clif_displaymessage(fd, "Auto Attack has been deactivated.");
	}
	else
	{
		sd->state.autoattack = 1;
		add_timer(gettick()+1000,autoattack_timer,sd->bl.id,0);
		clif_displaymessage(fd, "Auto Attack activated.");
	}
	return 0;
}



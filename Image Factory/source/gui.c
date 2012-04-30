#include "types.h"

#define GRIDSIZE 75

//Declarations
sidebar_list_t *_sidebarTop        = 0;
workspace_panel_t *_workSpacePanel = 0;

PANEL *_activeConnector[2]         = {0,0};

int _scrollValue                   = 0;
int _sidebarEntryNumber            = 2;
int _connectingFunctions           = 0;
int _connectedType                 = 0;
int _mouseStartPosX                = 0;
int _mouseStartPosY                = 0;
int _actFrames                     = 0;
int _highestOrder                  = 1;
float _workSpaceScrollX            = 0;
float _workSpaceScrollY            = 0;
float _workSpaceAlpha              = 0;

PANEL *_actConnectingPan           = 0;
PANEL *_finalImagePan              = 0;
PANEL *_renderedImagePan           = 0;

BMAP *_sidebarTop_b                = "sidebar_top.png";
BMAP *_sidebarContent_b            = "sidebar_entry.png";
BMAP *_sidebarEnd_b                = "sidebar_end.png";
BMAP *_functionBack_b              = "function_background_alpha.png";
BMAP *_functionConnect_b           = "function_connector.png";
BMAP *_functionConnectUse_b        = "function_connector_inuse.png";
BMAP *_functionCloseOff_b          = "function_close_off.png";
BMAP *_functionCloseOn_b           = "function_close_on.png";
BMAP *_finalImageBack_b            = "finalimage_background.png";
BMAP *_variabeleBack_b             = "variable_background.png";
BMAP *_userImageBack_b             = "user_image_background.png";
BMAP *_dragIcon_b                  = "drag_icon.png";
BMAP *_linesRender_b               = 0;
BMAP *_linesRenderProcessed_b      = 0;

FONT *_sidebarFat_f                = "Calibri#16";
FONT *_sidebarNormal_f             = "Calibri#14";

PANEL *_linesPanel = 
{
	layer = 800;
	flags = SHOW | UNTOUCHABLE;
}

PANEL *_clearPanel = 
{
	size_x = 1280;
	size_y = 720;
	flags = UNTOUCHABLE;
}

//Prototypes
void updateGUI();
void _markInkey(PANEL *panel);
void generateUserVariable();
void generateUserVariable(int posX, int posY);
void generateUserVariable(float value, int posX, int posY);
void generateFunctionPanel(filter_header_t* header, int xPos, int yPos);
void _showConnectLines(var num, PANEL *pan, var num2, PANEL *pan2, variable_t *variable, filter_t *filter, int filterInNum);

//Events
void _sidebarEndEvent()
{
	if(_scrollValue == -_sidebarEntryNumber*36 + 72)
	_scrollValue = 36;
	else
	_scrollValue = -99999;
	updateGUI();
}

void _dragEvent(PANEL* panel)
{
	if((mouse_pos.x > 7) && (mouse_pos.x < 237))
	{
		PANEL *_dragPanel   = pan_create("flags |= SHOW", 600);
		_dragPanel->bmap    = _dragIcon_b;
		_dragPanel->scale_x = 0.001;
		_dragPanel->scale_y = 0.001;
		while(mouse_left)
		{
			if((_dragPanel->scale_x < 1.1)&&(_dragPanel->skill_x == 0))
			{
				_dragPanel->scale_x += time_step*1.5;
				_dragPanel->scale_y += time_step*1.5;
			}
			else
			{
				_dragPanel->skill_x = 1;
				if(_dragPanel->scale_x > 1)
				{
					_dragPanel->scale_x -= time_step*0.5;
					_dragPanel->scale_y -= time_step*0.5;
				}
			}
			_dragPanel.pos_x = mouse_pos.x - 44*_dragPanel->scale_x;
			_dragPanel.pos_y = mouse_pos.y - 44*_dragPanel->scale_x;
			draw_obj(_dragPanel);
			proc_mode = PROC_LATE;
			wait(1);
		}
		ptr_remove(_dragPanel);
		sidebar_list_t* temp_entry;
		for(temp_entry = _sidebarTop->next; temp_entry->next != 0; temp_entry = temp_entry->next);
		if(!((mouse_pos.x > 7) && (mouse_pos.x < 237) && (mouse_pos.y < temp_entry->panel->pos_y + 36)))
		{
			sidebar_list_t* temp_entry;
			for(temp_entry = _sidebarTop->next; temp_entry->panel != panel; temp_entry = temp_entry->next);
			generateFunctionPanel(temp_entry->header, mouse_pos.x, mouse_pos.y);
		}
	}
}

void _dragFunctionPanel(PANEL *panel)
{
	if(panel.skill_x == -1)
		return;
	int offset[2];
	offset[0] = mouse_pos.x - panel.pos_x;
	offset[1] = mouse_pos.y - panel.pos_y;
	workspace_panel_t* temp_wp;
	for(temp_wp = _workSpacePanel; temp_wp->panel != panel; temp_wp = temp_wp->next);
	temp_wp->layerordernum = _highestOrder + 1;
	while((mouse_left != NULL)&&(panel != NULL))
	{
		temp_wp->posX = mouse_pos.x - offset[0] - _workSpaceScrollX;
		temp_wp->posY = mouse_pos.y - offset[1] - _workSpaceScrollY;
		wait(1);
	}
	if(key_alt)
	{
		float startPosX = temp_wp->posX;
		float startPosY = temp_wp->posY;
		float finalPosX = integer((temp_wp->panel->pos_x + temp_wp->panel->size_x/2 - _workSpaceScrollX)/GRIDSIZE + 0.5)*GRIDSIZE - temp_wp->panel->size_x/2;
		float finalPosY = integer((temp_wp->panel->pos_y + temp_wp->panel->size_y/2 - _workSpaceScrollY)/GRIDSIZE + 0.5)*GRIDSIZE - temp_wp->panel->size_y/2;
		double lerp_d = 0;
		while(lerp_d < 1)
		{
			temp_wp->posX = startPosX * (1 - lerp_d) +  finalPosX * lerp_d;
			temp_wp->posY = startPosY * (1 - lerp_d) +  finalPosY * lerp_d;
			lerp_d += time_step / 2;
			wait(1);
		}
		temp_wp->posX = finalPosX;
		temp_wp->posY = finalPosY;
	}
}

void _removeFunctionPanel(var num, PANEL* panel)
{
	if(proc_status(_removeFunctionPanel))
		return;
	if(panel.skill_x != 99)
	filter_destroy((filter_t*)(panel->skill_y));
	else
	variable_destroy((variable_t*)(panel->skill_y));
	panel.skill_x     = -1;
	wait(1);
	panel->skill_y    = 0;
	int positionOffset[2];
	positionOffset[0] = panel->pos_x;
	positionOffset[1] = panel->pos_y;
	while(panel->skill_y != 2)
	{
		if((panel->scale_x < 1.2)&&(panel->skill_y == 0))
		{
			panel->scale_x += time_step*1;
			panel->scale_y += time_step*1;
		}
		else
		{
			panel->skill_y = 1;
			if(panel->scale_x > 0.01)
			{
				panel->scale_x -= time_step*2;
				panel->scale_y -= time_step*2;
			}
			else
			panel->skill_y = 2;
		}
		if(panel->scale_x <= 0)
		{
			panel->scale_x = 0.001;
			panel->scale_y = 0.001;
		}
		panel->pos_x = positionOffset[0] + (1-panel->scale_x)*panel->size_x*0.5;
		panel->pos_y = positionOffset[1] + (1-panel->scale_y)*panel->size_y*0.5;
		panel->alpha = panel->scale_x * 100;
		wait(1);
	}
	if(_workSpacePanel->panel == panel)
	{
		workspace_panel_t* temp_wp = _workSpacePanel;
		_workSpacePanel = _workSpacePanel->next;
		sys_free(_workSpacePanel);
	}
	else
	{
		workspace_panel_t* temp_wp;
		for(temp_wp = _workSpacePanel; temp_wp != NULL; temp_wp = temp_wp->next)
		{
			if(temp_wp->next)
			if(temp_wp->next->panel == panel)
			{
				workspace_panel_t* temp2_wp = temp_wp->next;
				temp_wp->next = temp_wp->next->next;
				sys_free(temp2_wp);
				break;
			}
		}
	}
	ptr_remove(panel);
	panel = 0;
}

void _getFilterVariables(variable_t **output, filter_t **input, int *inputNum, workspace_panel_t *panel, int num)
{
	int type = 0;
	if(num == 0)
	if(panel->isinput)
	type = 1;
	if(num > 0)
	if((panel->isinput)[num])
	type = 1;
	if(type == 1)
	{
		*input  = panel->filter;
		*inputNum = num;
		return;
	}
	else
	{
		if(panel->variable)
		{
			*output = panel->variable;
			return;
		}
		else
		{
			*output = filter_getOutput(panel->filter, num-panel->filter->header->numIn);
			return;
		}
	}
}

int variableHasConnections(variable_t *v)
{
	int j;
	int connectionNumber = 0;
	for(j=0; j<kVariableMaxConnections; j++)
	{
		filter_t *filter = (v->connection)[j];
				
		if(filter)
			connectionNumber++;
	}

	return connectionNumber;
}

void _connectFunctions(var num, PANEL* panel)
{
	variable_t *output = 0;
	filter_t *input    = 0;
	int inputNum       = -1;
	int thisType       = 0;

	workspace_panel_t* calling_wp;
	for(calling_wp = _workSpacePanel; calling_wp != NULL; calling_wp = calling_wp->next)
	{
		if(calling_wp->panel == panel)
		{
			_getFilterVariables(&output, &input, &inputNum, calling_wp, num-1);
			if(output)
			thisType = 1;
			break;
		}
	}
	if((!calling_wp)&&(panel == _finalImagePan))
	{
		input = library_outputFilter;
		inputNum = 0;
	}
	workspace_panel_t* selected_wp = 0;
	if(_actConnectingPan)
	{
		for(selected_wp = _workSpacePanel; selected_wp != NULL; selected_wp = selected_wp->next)
		{
			if(selected_wp->panel == _actConnectingPan)
			{
				_getFilterVariables(&output, &input, &inputNum, selected_wp, _connectingFunctions-1);
				break;
			}
		}
	}
	if((!selected_wp)&&(_actConnectingPan == _finalImagePan))
	{
		input = library_outputFilter;
		inputNum = 0;
	}

	if(button_state(panel, num, -1) == 1)
	{
		if(_connectingFunctions > 0)
		{
			if(panel != _actConnectingPan)
			{
				if((input != output)&&(input != NULL)&&(output != NULL))
				{
					if(!filter_connect(input, output, inputNum))
					{
						if(thisType)
						{
							if(!variableHasConnections(output))
								button_state(panel, num, 0);
							button_state(_actConnectingPan, _connectingFunctions, 0);
						}
						else
						{
							if(!variableHasConnections(output))
								button_state(_actConnectingPan, _connectingFunctions, 0);
							button_state(panel, num, 0);
						}
						_actConnectingPan    = 0;
						_connectingFunctions = 0;
					}
					else
					{
						_showConnectLines(num, panel, _connectingFunctions, _actConnectingPan, output, input, inputNum);
						_actConnectingPan    = 0;
						_connectingFunctions = 0;

					}
				}
				else
				{

					button_state(panel, num, 0);
					button_state(_actConnectingPan, _connectingFunctions, 0);
				}
			}
			else
			{
				button_state(panel, num, 0);
				button_state(_actConnectingPan, _connectingFunctions, 0);
			}
		}
		else
		{
			_connectingFunctions = num;
			_actConnectingPan    = panel;
		}
	}
	else
	{
		if(thisType)
		{
			if(variableHasConnections(output)) //> 1)
			{
				button_state(panel, num, 1);
				if(_connectingFunctions > 0)
				{
					if(panel != _actConnectingPan)
					{
						if((input != output)&&(input != NULL)&&(output != NULL))
						{
							if(!filter_connect(input, output, inputNum))
							{
								if(thisType)
								{
									if(!variableHasConnections(output))
										button_state(panel, num, 0);
									button_state(_actConnectingPan, _connectingFunctions, 0);
								}
								else
								{
									if(!variableHasConnections(output))
										button_state(_actConnectingPan, _connectingFunctions, 0);
									button_state(panel, num, 0);
								}
							}
							else
							{
								_showConnectLines(num, panel, _connectingFunctions, _actConnectingPan, output, input, inputNum);
								_actConnectingPan    = 0;
								_connectingFunctions = 0;

							}
						}
						else
						{
							if(input != 0)
							{
								button_state(panel, num, 0);
								button_state(_actConnectingPan, _connectingFunctions, 0);
								_actConnectingPan    = 0;
								_connectingFunctions = 0;
							}
							else
							{
								_actConnectingPan    = 0;
								_connectingFunctions = 0;
							}
						}
					}
					else
					{
						if(input != 0)
						{
							button_state(panel, num, 0);
							button_state(_actConnectingPan, _connectingFunctions, 0);
							_actConnectingPan    = 0;
							_connectingFunctions = 0;
						}
						else
						{
							_actConnectingPan    = 0;
							_connectingFunctions = 0;
						}
					}
				}
				else
				{
					_connectingFunctions = num;
					_actConnectingPan    = panel;
				}
			}
			/*else if(variableHasConnections(output) == 1)
			{
				int j;
				for(j=0; j<kVariableMaxConnections; j++)
				{
					filter_t *filter = (output->connection)[j];
							
					if(filter)
					{
						int i;
						for(i = 0; i<filter->header->numIn; i++)
						{
							if((filter->input)[i] == output)
							filter_unconnect(filter,i);
						}
					}
				}
			}*/
			else
			{
				_connectingFunctions = 0;
				_actConnectingPan    = 0;
			}
		}
		else
		{
			filter_unconnect(input, inputNum);
		}
	}
}

void _finalImageMoveEvent(PANEL* panel)
{
	inkey_active	=	0;
	if(proc_status(_finalImageMoveEvent))
		return;
	if(panel.pos_x > screen_size.x - 100)
	{
		while(panel.pos_x > screen_size.x  - 400)
		{
			panel.pos_x -= 55*time_step;
			wait(1);
		}
		while(panel.pos_x < screen_size.x  - 345)
		{
			panel.pos_x += 25*time_step;
			wait(1);
		}
	}
	else
	{
		while(panel.pos_x < screen_size.x - 80)
		{
			panel.pos_x += 50*time_step;
			wait(1);
		}
	}
}

void _checkMouse()
{
	wait(1);
	while((inkey_active != 0)&&(mouse_left != 1)) wait(1);
	inkey_active	=	0;
}

void _mouseClickEvent()
{
	int changeValue = 0;
	if(proc_status(_mouseClickEvent))
		return;
	if(mouse_panel)
	{
		if(mouse_panel->skill_x == 99)
			changeValue = 1;
		else
			return;
	}
	var time_done = 0;
	while(mouse_left)
	{
		time_done += time_frame/16;
		wait(1);
	}
	if(time_done > 0.3)
		return;
	time_done = 0;
	while(!mouse_left)
	{
		time_done += time_frame/16;
		wait(1);
	}
	if(time_done > 0.3)
		return;
	time_done = 0;
	while(mouse_left)
	{
		time_done += time_frame/16;
		wait(1);
	}
	if(time_done > 0.3)
		return;
	
	if((changeValue == 0)&&(!mouse_panel))
	{
		generateUserVariable(mouse_pos.x, mouse_pos.y);
	}
	else
	{
		if(mouse_panel->skill_x != 99)
			return;
		float startFloat = variable_getFloat((variable_t*)mouse_panel->skill_y);
		STRING *_inputString	= str_create("          ");
		pan_setstring(mouse_panel, 1, 5, 5, _sidebarNormal_f, _inputString);
		_markInkey(mouse_panel);
		PANEL *pan = mouse_panel;
		inkey(_inputString);
		if((result != 13)&&(result != 1))
		{
			str_cpy(_inputString, str_for_float(NULL, startFloat));
			str_trunc(_inputString, str_len(_inputString)-10);
		}
		else 
		{
			str_cpy(_inputString, str_for_float(NULL, str_to_float(_inputString)));
			str_trunc(_inputString, str_len(_inputString)-10);
			variable_setType(((variable_t*)pan->skill_y), float, str_to_float(_inputString));
			variable_invalidate((variable_t*)pan->skill_y);
		}
	}
}

//Funktionen
void generateSidebarEntry(filter_header_t* entry)
{
	PANEL* panel	= pan_create("", 1000);
	pan_setstring(panel, 0, 20,	 0, _sidebarFat_f, 		entry->name);
	pan_setcolor(panel, 1,1, _vec(100,100,100));
	if(str_stri(entry->description, "\n") == 0)
		pan_setstring(panel, 0, 38,	16, _sidebarNormal_f,	entry->description);
	else
	{
		STRING* temp_str = str_create(""); //str_trunc(str_cpy(str_create(""), entry->description), str_stri(entry->description, "\n"));
		str_cpy(temp_str, entry->description);
		str_trunc(temp_str, str_len(entry->description)-str_stri(entry->description, "\n"));
		pan_setstring(panel, 0, 38,	12, _sidebarNormal_f, temp_str);
		str_clip(entry->description, str_stri(entry->description, "\n"));
		pan_setstring(panel, 0, 38,	20, _sidebarNormal_f, entry->description);
	}
	pan_setcolor(panel, 1,2, _vec(130,130,130));
	pan_setcolor(panel, 1,3, _vec(130,130,130));
	set(panel, SHOW);
	panel->bmap                     = _sidebarContent_b;
	panel->event                    = _dragEvent;
	sidebar_list_t* temp_list_entry = sys_malloc(sizeof(sidebar_list_t));
	temp_list_entry->next           = 0;
	temp_list_entry->panel          = panel;
	temp_list_entry->header         = entry; 
	sidebar_list_t* temp_entry;
	for(temp_entry                  = _sidebarTop; temp_entry->next->next != 0; temp_entry = temp_entry->next);
	temp_list_entry->next           = temp_entry->next;
	temp_entry->next                = temp_list_entry;
	_sidebarEntryNumber++;
}

void generateUserImagePreview(BMAP *bmap, int posX, int posY)
{
	PANEL* panel                 = pan_create("", 500 + _highestOrder);
	set(panel, SHOW);
	panel->bmap                  = bmap_createblack(100,80,32);
	bmap_blit(panel->bmap, _userImageBack_b, NULL, NULL);
	bmap_blit(panel->bmap, bmap, vector(8,8,0), vector(64,64,0));
	panel->event                 = _dragFunctionPanel;
	pan_setbutton(panel, 0, 2,78,41, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
	pan_setbutton(panel, 0, 0,78,23, _functionCloseOn_b,_functionCloseOff_b,_functionCloseOn_b,_functionCloseOff_b,_removeFunctionPanel,0,0);
	panel->skill_x               = 99;
	panel->skill_y               = (var)variable_createBitmap(bmap);
	panel->pos_x                 = posX;
	panel->pos_y                 = posY;
	workspace_panel_t* wPanel_wp = sys_malloc(sizeof(workspace_panel_t));
	wPanel_wp->isinput           = sys_malloc(sizeof(int));
	wPanel_wp->isinput           = 0;
	wPanel_wp->filter            = 0;
	wPanel_wp->variable          = variable_createBitmap(bmap);
	wPanel_wp->panel             = panel;
	wPanel_wp->posX              = posX - _workSpaceScrollX;
	wPanel_wp->posY              = posY - _workSpaceScrollY;
	wPanel_wp->next              = 0;
	wPanel_wp->layerordernum			 = _highestOrder+1;
	if(_workSpacePanel == 0)
	_workSpacePanel = wPanel_wp;
	else
	{
		workspace_panel_t* temp_wp;
		for(temp_wp = _workSpacePanel; temp_wp->next != 0; temp_wp = temp_wp->next);
		temp_wp->next = wPanel_wp;
	}
}

void generateUserImagePreview(BMAP *bmap)
{
	generateUserImagePreview(bmap, screen_size.x / 2 - 50, screen_size.y / 2 - 40);
}

void generateUserVariable(float value, int posX, int posY)
{
	button_state(_actConnectingPan, _connectingFunctions, 0);
	STRING *_inputString         = str_create("");
	PANEL *_variableInput        = pan_create("", 500 + _highestOrder);
	_variableInput->bmap         = _variabeleBack_b;
	pan_setstring(_variableInput, 0, 5, 5, _sidebarNormal_f, _inputString);
	pan_setbutton(_variableInput, 0, 2,80,5, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
	pan_setbutton(_variableInput, 0, 0,100,5, _functionCloseOn_b,_functionCloseOff_b,_functionCloseOn_b,_functionCloseOff_b,_removeFunctionPanel,0,0);
	_variableInput->pos_x        = posX;
	_variableInput->pos_y        = posY;
	set(_variableInput, SHOW);
	str_cpy(_inputString, str_for_float(NULL, value));
	str_trunc(_inputString, str_len(_inputString)-10);
	_variableInput->skill_x      = 99;
	_variableInput->event        = _dragFunctionPanel;
	workspace_panel_t* wPanel_wp = sys_malloc(sizeof(workspace_panel_t));
	wPanel_wp->isinput           = sys_malloc(sizeof(int));
	wPanel_wp->isinput           = 0;
	wPanel_wp->filter            = 0;
	wPanel_wp->variable          = variable_createFloat(value);
	_variableInput->skill_y      = (var)wPanel_wp->variable;
	wPanel_wp->panel             = _variableInput;
	wPanel_wp->posX              = posX - _workSpaceScrollX;
	wPanel_wp->posY              = posY - _workSpaceScrollY;
	wPanel_wp->next              = 0;
	wPanel_wp->layerordernum			 = _highestOrder+1;
	if(_workSpacePanel == 0)
	_workSpacePanel = wPanel_wp;
	else
	{
		workspace_panel_t* temp_wp;
		for(temp_wp = _workSpacePanel; temp_wp->next != 0; temp_wp = temp_wp->next);
		temp_wp->next = wPanel_wp;
	}
}

void generateUserVariable(int posX, int posY)
{
	button_state(_actConnectingPan, _connectingFunctions, 0);
	STRING *_inputString	= str_create("          ");
	PANEL *_variableInput	= pan_create("", 500 + _highestOrder);
	_variableInput->bmap 	= _variabeleBack_b;
	pan_setstring(_variableInput, 0, 5, 5, _sidebarNormal_f, _inputString);
	pan_setbutton(_variableInput, 0, 2,80,5, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
	pan_setbutton(_variableInput, 0, 0,100,5, _functionCloseOn_b,_functionCloseOff_b,_functionCloseOn_b,_functionCloseOff_b,_removeFunctionPanel,0,0);
	_variableInput->pos_x	= posX;
	_variableInput->pos_y	= posY;
	set(_variableInput, SHOW);
	_checkMouse();
	result = inkey(_inputString);
	if((result != 13)&&(result != 1))
	{
		ptr_remove(_variableInput);
		ptr_remove(_inputString);
	}
	else 
	{
		str_cpy(_inputString, str_for_float(NULL, str_to_float(_inputString)));
		str_trunc(_inputString, str_len(_inputString)-10);
		_variableInput->skill_x	= 99;
		_variableInput->event 	= _dragFunctionPanel;

		workspace_panel_t* wPanel_wp = sys_malloc(sizeof(workspace_panel_t));
		wPanel_wp->isinput           = sys_malloc(sizeof(int));
		wPanel_wp->isinput           = 0;
		wPanel_wp->filter            = 0;
		wPanel_wp->variable          = variable_createFloat(str_to_float(_inputString));
		_variableInput->skill_y      = (var)wPanel_wp->variable;
		wPanel_wp->panel             = _variableInput;
		wPanel_wp->posX              = _variableInput->pos_x - _workSpaceScrollX;
		wPanel_wp->posY              = _variableInput->pos_y - _workSpaceScrollY;
		wPanel_wp->next              = 0;
		wPanel_wp->layerordernum			 = _highestOrder+1;
		if(_workSpacePanel == 0)
		_workSpacePanel = wPanel_wp;
		else
		{
			workspace_panel_t* temp_wp;
			for(temp_wp = _workSpacePanel; temp_wp->next != 0; temp_wp = temp_wp->next);
			temp_wp->next = wPanel_wp;
		}
	}
}

void generateUserVariable()
{
	generateUserVariable(screen_size.x / 2- 60, screen_size.y / 2 - 12);
}

void generateFunctionPanel(filter_header_t* header, int xPos, int yPos)
{
	BMAP* final_b = bmap_createblack(150, 64 + maxv(header->numIn, header->numOut)*25, 32);
	bmap_fill(final_b, _vec(55,41,34), 100);
	bmap_blitpart(final_b, _functionBack_b, nullvector, NULL, NULL, _vec(12,12,0));
	bmap_blitpart(final_b, _functionBack_b, _vec(138,0,0), NULL, _vec(13,0,0), _vec(12,12,0));
	bmap_blitpart(final_b, _functionBack_b, _vec(0,64 + maxv(header->numIn, header->numOut)*25-12,0), NULL, _vec(0,13,0), _vec(12,12,0));
	bmap_blitpart(final_b, _functionBack_b, _vec(138,64 + maxv(header->numIn, header->numOut)*25-12,0), NULL, _vec(13,13,0), _vec(12,12,0));
	
	workspace_panel_t* wPanel_wp = sys_malloc(sizeof(workspace_panel_t));
	wPanel_wp->isinput           = sys_malloc((header->numIn + header->numOut) * sizeof(int));
	PANEL* _functionPanel        = pan_create("", 500 + _highestOrder);
	wPanel_wp->filter            = filter_create(header);
	_functionPanel->skill_y      = (var)wPanel_wp->filter;
	pan_setstring(_functionPanel, 0,8,16, font_create("Calibri#20b"),	header->name);
	int i;
	for(i = 0; i<header->numIn; i++)
	{
		pan_setbutton(_functionPanel, 0, 2,8,45 + 25*i, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
		pan_setstring(_functionPanel, 0,30,45+25*i, _sidebarNormal_f,	(((header->namesIn)[i])));
		(wPanel_wp->isinput)[i]   = 1;
	}
	for(i = 0; i<header->numOut; i++)
	{
		pan_setbutton(_functionPanel, 0, 2,126,45 + 25*i, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
		pan_setstring(_functionPanel, 0,120-str_width((header->namesOut)[i], _sidebarNormal_f),45+25*i, _sidebarNormal_f,	(header->namesOut)[i]);
		(wPanel_wp->isinput)[header->numIn+i] = 0;
	}
	pan_setbutton(_functionPanel, 0, 0,130,4, _functionCloseOn_b,_functionCloseOff_b,_functionCloseOn_b,_functionCloseOff_b,_removeFunctionPanel,0,0);
	set(_functionPanel, SHOW|TRANSLUCENT);
	_functionPanel->bmap   = final_b;
	_functionPanel->pos_x  = xPos - 75;
	_functionPanel->pos_y  = yPos - ((64 + maxv(header->numIn, header->numOut)*25)/2);
	_functionPanel->size_x = 150;
	_functionPanel->size_y = 64 + maxv(header->numIn, header->numOut)*25;
	_functionPanel->event  = _dragFunctionPanel;
	_functionPanel->alpha  = 50;
	wPanel_wp->panel       = _functionPanel;
	wPanel_wp->posX        = _functionPanel->pos_x - _workSpaceScrollX;
	wPanel_wp->posY        = _functionPanel->pos_y - _workSpaceScrollY;
	wPanel_wp->variable    = 0;
	wPanel_wp->layerordernum       = _highestOrder+1;
	if(key_alt)
	{
		wPanel_wp->posX = integer((wPanel_wp->panel->pos_x + wPanel_wp->panel->size_x/2 - _workSpaceScrollX)/GRIDSIZE + 0.5)*GRIDSIZE - wPanel_wp->panel->size_x/2;
		wPanel_wp->posY = integer((wPanel_wp->panel->pos_y + wPanel_wp->panel->size_y/2 - _workSpaceScrollY)/GRIDSIZE + 0.5)*GRIDSIZE - wPanel_wp->panel->size_y/2;
		updateGUI();
	}
	wPanel_wp->next              = 0;
	if(_workSpacePanel == 0)
	_workSpacePanel = wPanel_wp;
	else
	{
		workspace_panel_t* temp_wp;
		for(temp_wp = _workSpacePanel; temp_wp->next != 0; temp_wp = temp_wp->next);
		temp_wp->next = wPanel_wp;
	}
}

void _markInkey(PANEL *panel)
{
	while(!inkey_active) wait(1);
	double i;
	VECTOR temp;
	while(inkey_active)
	{
		vec_lerp(temp, vector(255,255,255), vector(55,41,34), i);
		i = 0.5 + sin(total_frames*4)/2;
		pan_setcolor(panel, 1, 1, temp);
		wait(1);
	}
	pan_setcolor(panel, 1, 1, vector(255,255,255));
}
void drawBezier(VECTOR *vec1, VECTOR *vec2, VECTOR *vec3, VECTOR *vec4)
{
	double p1[2], p2[2], p3[2], p4[2];
	double t;
	double factor;
	double position[2], tempPos[2];

	p1[0] = vec1->x;
	p1[1] = vec1->y;
	p2[0] = vec2->x;
	p2[1] = vec2->y;
	p3[0] = vec3->x;
	p3[1] = vec3->y;
	p4[0] = vec4->x;
	p4[1] = vec4->y;

	draw_line(vec1, NULL, 70);
	draw_line(vec1, _vec(240,234,227), 100);
	for(t = 0; t<=1; t+=0.04)
	{
		position[0] = 0;	position[1] = 0;

		factor = (1 - t) * (1 - t) * (1 - t);
		tempPos[0] = p1[0] * factor;	tempPos[1] = p1[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = 3 * (1 - t) * (1 - t) * t;
		tempPos[0] = p2[0] * factor;	tempPos[1] = p2[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = 3 * (1 - t) * t * t;
		tempPos[0] = p3[0] * factor;	tempPos[1] = p3[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = t * t * t;
		tempPos[0] = p4[0] * factor;	tempPos[1] = p4[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];

		draw_line(vector((float)position[0], (float)position[1],0), _vec(240,234,227), 100);
	}
	draw_line(vec4, _vec(240,234,227), 100);
	draw_line(vec4, NULL, 70);
}
void _showConnectLines(var num, PANEL *pan, var num2, PANEL *panX, variable_t *variable, filter_t *filter, int filterInNum)
{
	_connectingFunctions	= 0;
	_actConnectingPan		= 0;
	PANEL* pan1 = pan;
	PANEL* pan2 = panX;
	VECTOR vec1, vec2, vec3, vec4;
	VECTOR pos, lastPos, temp;
	double t;
	while((pan1.skill_x != -1)&&(pan2.skill_x != -1))
	{
		if(_actFrames != total_frames)
		{
			_actFrames = total_frames;
			//draw_quad(NULL, nullvector, NULL, vector(1280,720,0), NULL, vector(77,64,58),100,0);
			//set(_clearPanel, LIGHT|TRANSLUCENT);
			//vec_set(_clearPanel->blue, vector(77,64,58));
			//_clearPanel->alpha = 0;
			//_clearPanel->target_map = _linesRender_b;
			//draw_obj(_clearPanel);
		}
		while(!window_focus) wait(1);
		if((button_state(pan1, num, -1) == 0)||(button_state(pan2, num2, -1) == 0))
		{
			wait(2);
			if((button_state(pan1, num, -1) == 0)||(button_state(pan2, num2, -1) == 0))
			{
				if(!variableHasConnections(variable))
				{
					button_state(pan, num, 0);
					button_state(panX, num2, 0);
				}
				return;
			}
		}
		pan_getpos(pan1, 3, num, &vec1);
		vec1.z = 0;
		pan_getpos(pan2, 3, num2, &vec4);
		vec4.z = 0;
		vec_set(vec2, vec1);
		vec_set(vec3, vec4);
		/*if((vec1.x + pan1.pos_x + 8) < (vec4.x + pan2.pos_x + 8))
		vec2.x += (((vec1.x + pan1.pos_x + 8) - (vec4.x + pan2.pos_x + 8)) * 0.5);
		else
		vec2.x += (((vec4.x + pan2.pos_x + 8) - (vec1.x + pan1.pos_x + 8)) * 0.5);
		vec_set(vec3, vec4);
		if((vec1.x + pan1.pos_x + 8) > (vec4.x + pan2.pos_x + 8))
		vec3.x += (((vec1.x + pan1.pos_x + 8) - (vec4.x + pan2.pos_x + 8)) * 0.5);
		else
		vec3.x += (((vec4.x + pan2.pos_x + 8) - (vec2.x + pan1.pos_x + 8)) * 0.5);*/
		vec2.x -= (((vec1.x + pan1.pos_x + 8) - (vec4.x + pan2.pos_x + 8)) * 0.5);
		vec3.x += (((vec1.x + pan1.pos_x + 8) - (vec4.x + pan2.pos_x + 8)) * 0.5);
		vec1.x += pan1.pos_x + 8;
		vec1.y += pan1.pos_y + 8;
		vec2.x += pan1.pos_x + 8;
		vec2.y += pan1.pos_y + 8;
		vec3.x += pan2.pos_x + 8;
		vec3.y += pan2.pos_y + 8;
		vec4.x += pan2.pos_x + 8;
		vec4.y += pan2.pos_y + 8;
		//bmap_rendertarget(_linesRender_b, 0, 0);
		/*draw_line(vec1, NULL, 70);
		draw_line(vec1, _vec(240,234,227), 70);
		vec_set(lastPos, vec1);
		for(t = 0; t < 1; t += time_step/6)
		{
			vec_zero(pos);
			vec_set(temp, vec1);
			vec_scale(temp, (1-t)*(1-t)*(1-t));
			vec_add(pos, temp);
			vec_set(temp, vec2);
			vec_scale(temp, 3*t*(1-t)*(1-t));
			vec_add(pos, temp);
			vec_set(temp, vec3);
			vec_scale(temp, 3*t*t*(1-t));
			vec_add(pos, temp);
			vec_set(temp, vec4);
			vec_scale(temp, t*t*t);
			vec_add(pos, temp);
			draw_line(pos, _vec(240,234,227), 70);
			/*draw_line(_vec(lastPos.x+0.3,lastPos.y+0.3,0), NULL, 35);
			draw_line(_vec(pos.x+0.3,pos.y+0.3,0), _vec(240,234,227), 35);
			draw_line(_vec(lastPos.x,lastPos.y,0), NULL, 35);
			draw_line(_vec(pos.x+0.3,pos.y+0.3,0), _vec(240,234,227), 35);
			draw_line(_vec(lastPos.x-0.3,lastPos.y-0.5,0), NULL, 35);
			draw_line(_vec(pos.x-0.3,pos.y-0.3,0), _vec(240,234,227), 35);
			draw_line(_vec(lastPos.x,lastPos.y,0), NULL, 35);
			draw_line(_vec(pos.x-0.3,pos.y-0.3,0), _vec(240,234,227), 35);
			draw_line(pos, NULL, 70);*
			vec_set(lastPos, pos);
		}
		draw_line(vec4, _vec(240,234,227), 70);
		draw_line(vec4, NULL, 70);*/
		drawBezier(vec1,vec2,vec3,vec4);
		//bmap_rendertarget(NULL, 0, 0);
		proc_mode = PROC_EARLY;
		wait(1);
	}
	wait(1);
	if(!variableHasConnections(variable))
	{
		button_state(pan, num, 0);
		button_state(panX, num2, 0);
	}
	if(!filter_getInput(filter, filterInNum))
	{
		button_state(pan, num, 0);
		button_state(panX, num2, 0);
	}
	/*PANEL* pan1 = pan;
	PANEL* pan2 = panX;
	VECTOR vec1, vec2;
	vec_zero(vec1);
	vec_zero(vec2);
	while((pan1.skill_x != -1)&&(pan2.skill_x != -1))
	{
		while(!window_focus) wait(1);
		if((button_state(pan1, num, -1) == 0)||(button_state(pan2, num2, -1) == 0))
		{
			wait(2);
			if((button_state(pan1, num, -1) == 0)||(button_state(pan2, num2, -1) == 0))
			{
				button_state(pan1, num, 0);
				button_state(pan2, num2, 0);
				return;
			}
		}
		pan_getpos(pan1, 3, num, &vec1);
		vec1.z = 0;
		pan_getpos(pan2, 3, num2, &vec2);
		vec2.z = 0;
		vec1.x += pan1.pos_x + 8;
		vec1.y += pan1.pos_y + 8;
		vec2.x += pan2.pos_x + 8;
		vec2.y += pan2.pos_y + 8;
		draw_line(vec1, NULL, 70);
		draw_line(vec1, _vec(240,234,227), 70);
		draw_line(vec2, _vec(240,234,227), 70);
		draw_line(vec2, NULL, 70);
		proc_mode = PROC_EARLY;
		wait(1);
	}
	button_state(pan1, num, 0);
	button_state(pan2, num2, 0);*/
}
void initGUI()
{
	PANEL* panel        = pan_create("", 1000);
	panel->bmap         = _sidebarTop_b;
	set(panel, SHOW);
	_sidebarTop         = sys_malloc(sizeof(sidebar_list_t));
	_sidebarTop->next   = 0;
	_sidebarTop->panel  = panel;
	_sidebarTop->header = 0;
	PANEL* panel        = pan_create("", 1000);
	panel->bmap         = _sidebarEnd_b;
	panel->event        = _sidebarEndEvent;
	set(panel, SHOW);
	sidebar_list_t* temp_list_entry = sys_malloc(sizeof(sidebar_list_t));
	temp_list_entry->next 	= 0;
	temp_list_entry->panel 	= panel;
	temp_list_entry->header = 0;
	_sidebarTop->next = temp_list_entry;
	
	_finalImagePan 			= pan_create("", 1000);
	_finalImagePan->bmap 	= _finalImageBack_b;
	set(_finalImagePan, SHOW);
	_finalImagePan->pos_x	= screen_size.x - 80;
	_finalImagePan->pos_y	= screen_size.y / 2 - 157;
	_finalImagePan->event 	= _finalImageMoveEvent;
	_finalImagePan->skill_y	= (var)library_outputFilter;
	pan_setbutton(_finalImagePan, 0, 2,16,149, _functionConnectUse_b,_functionConnect_b,_functionConnectUse_b,_functionConnect_b,_connectFunctions,0,0);
	
	_renderedImagePan        = pan_create("", 999);
	set(_renderedImagePan, SHOW);
	_renderedImagePan->event = _finalImageMoveEvent;
	
	on_mouse_left            = _mouseClickEvent;
	
	_linesRender_b           = bmap_createblack(1280,720,32);
	_linesPanel->bmap        = _linesRender_b;
}
void updateGUI()
{
	if(!_sidebarTop)
	return;
	if(!window_focus)
	{
		key_pressed(-1);
		return;
	}

	if((key_alt)||(_workSpaceAlpha > 0))
	{
		_workSpaceAlpha = clamp(_workSpaceAlpha + (time_step * 30 * (key_alt*2 - 1)),0,100);
		int i;
		for(i = cycle(_workSpaceScrollX,-GRIDSIZE/2,GRIDSIZE/2); i < screen_size.x; i += GRIDSIZE)
		{
			draw_line(_vec(i, -1, 0), NULL, _workSpaceAlpha);
			draw_line(_vec(i, screen_size.y + 1, 0), _vec(173,143,130), _workSpaceAlpha);
		}
		for(i = cycle(_workSpaceScrollY,-GRIDSIZE/2,GRIDSIZE/2); i < screen_size.y; i += GRIDSIZE)
		{
			draw_line(_vec(-1, i, 0), NULL, _workSpaceAlpha);
			draw_line(_vec(screen_size.x + 1, i, 0), _vec(173,143,130), _workSpaceAlpha);
		}
	}

	sidebar_list_t* temp_entry;
	for(temp_entry = _sidebarTop->next; temp_entry->next != 0; temp_entry = temp_entry->next);
	if((mouse_pos.x > 7) && (mouse_pos.x < 237) && (mouse_pos.y < temp_entry->panel->pos_y + 36))
	_scrollValue += mickey.z / 120 * 36;
	_scrollValue = clamp(_scrollValue, -_sidebarEntryNumber*36 + 72, 0);
	int i = 1;
	draw_obj(_sidebarTop->panel);
	for(temp_entry = _sidebarTop->next; temp_entry->next != 0; temp_entry = temp_entry->next, i++)
	{
		temp_entry->panel->pos_y = 36 * i + _scrollValue;
		if((temp_entry->panel->pos_y < 36)||(temp_entry->panel->pos_y >= screen_size.y-36))
		reset(temp_entry->panel, SHOW);
		else
		{
			set(temp_entry->panel, SHOW);
			draw_obj(temp_entry->panel);
		}
	}
	if(_sidebarEntryNumber * 36 + _scrollValue > screen_size.y)
	temp_entry->panel->pos_y = screen_size.y - 36;
	else
	temp_entry->panel->pos_y = 36 * i + _scrollValue;
	draw_obj(temp_entry->panel);
	draw_obj(_finalImagePan);
	_renderedImagePan.pos_x  = _finalImagePan.pos_x + 177 - (_renderedImagePan.size_x/2);
	_renderedImagePan.pos_y  = _finalImagePan.pos_y + 157 - (_renderedImagePan.size_y/2);
	draw_obj(_renderedImagePan);

	if(mouse_right == 1)
	{
		if((_mouseStartPosX == 0)&&(_mouseStartPosY == 0))
		{
			_mouseStartPosX = mouse_pos.x - _workSpaceScrollX;
			_mouseStartPosY = mouse_pos.y - _workSpaceScrollY;
		}
		_workSpaceScrollX = mouse_pos.x - _mouseStartPosX;
		_workSpaceScrollY = mouse_pos.y - _mouseStartPosY;
	}
	else
	{
		_mouseStartPosX	  = 0;
		_mouseStartPosY	  = 0;
	}
	workspace_panel_t* temp_wp;
	int workspace_panel_nr = 0;
	for(temp_wp = _workSpacePanel; temp_wp != 0; temp_wp = temp_wp->next)
	{
		temp_wp->panel->pos_x = temp_wp->posX + _workSpaceScrollX;
		temp_wp->panel->pos_y = temp_wp->posY + _workSpaceScrollY;
		workspace_panel_nr++;
	}
	int actLayer = 0, smallestOrder = 0, lastSmallOrder = 0;
	workspace_panel_t *temp_wp, *smallest_wp;
	for(i = 0; i < workspace_panel_nr; i++)
	{
		smallestOrder = 99999;
		for(temp_wp = _workSpacePanel; temp_wp != 0; temp_wp = temp_wp->next)
		{
			if((temp_wp->layerordernum < smallestOrder)&&(temp_wp->layerordernum > lastSmallOrder))
			{
				smallest_wp  = temp_wp;
				smallestOrder = temp_wp->layerordernum;
			}
		}
//		if(!smallest_wp)
//		error("!");		
//		smallest_wp->layerordernum = actLayer;
		layer_sort(smallest_wp->panel, 500+actLayer);
		lastSmallOrder = smallestOrder;
		actLayer++;
	}
	_highestOrder = smallestOrder;
	/*bmap_process(_linesRender_b, _linesRender_b, _matThicklines);
	bmap_process(_linesRender_b, _linesRender_b, _matSimpleBlur);
	/*_matFill->skill1 = floatv(0.2275);
	_matFill->skill2 = floatv(0.2510);
	_matFill->skill3 = floatv(0.3020);
	_matFill->skill4 = floatv(0.0);
	bmap_process(_linesRenderProcessed_b, NULL, _matFill);
	bmap_process(_linesRenderProcessed_b, _linesRender_b, _matThicklines);
	bmap_process(_linesRenderProcessed_b, _linesRenderProcessed_b, _matComplexBlur);
	bmap_process(_linesRenderProcessed_b, _linesRenderProcessed_b, _matComplexBlur);*/
}
-- this file has been proudly written in gvim ;)
win1 = {type="Window",
	x=200,
	y=100,
	colspacing=8.0,
	rowspacing=8.0,
	shadow=true,
	background=true,
	titlebar=true,
	caption="The Fu*k Java SFGUI Example",
	resize=true,
	widgets={
		{
			type="Label",
			text="Is c++ better?",
			spanx=3
		},
		{
			type="Button",
			text="Naturally",
			y=1
		},
		{
			type="Button",
			text="Of Course",
			x=1,y=1
		},
		{
			type="Button",
			text="Yes!",
			x=2,y=1,
			OnLeftClick="correct"
		},
		{
			type="RadioButton",
			text="Hmm1",
			x=0,y=2,
			down=true,
			group="one"
		},
		{
			type="RadioButton",
			text="Hmm2",
			x=1,y=2,
			group="one",
			down=true
		},
		{
			type="RadioButton",
			text="Hmm3",x=2,y=2,
			group="one"
		}
	}
}

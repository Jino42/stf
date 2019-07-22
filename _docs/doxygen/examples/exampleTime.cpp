while (!DisplayWindow::Get().exit()) {

    //Update
    Time::Get().update();

    //Pause management
    if (PAUSE || UNPAUSE)
        Time::Get().pause(!Time::Get().isPause());


    if (!Time::Get().isPause())
    {
        //Logical Loop
        while (Time::Get().shouldUpdateLogic()) {
            //Logic
        }

        //Render
        if (!Time::Get().isPause())
            MainGraphic::Get().render();
    }

}
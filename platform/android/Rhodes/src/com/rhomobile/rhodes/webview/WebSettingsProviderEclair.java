package com.rhomobile.rhodes.webview;

import android.content.Context;
import android.webkit.WebSettings;

import com.rhomobile.rhodes.util.ContextFactory;

public class WebSettingsProviderEclair extends WebSettingsProviderBase implements IWebSettingsProvider {
    public void fillSettings(WebSettings settings) {
        super.fillSettings(settings);
        settings.setDatabaseEnabled(true);

        String databasePath = ContextFactory.getContext().getDir("database", Context.MODE_PRIVATE).getPath();
        settings.setDatabasePath(databasePath);
    }
}
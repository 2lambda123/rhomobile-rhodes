﻿using System;
using System.Collections.Generic;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace <%= name.camel_case() %>Impl
{
    public class <%= name.camel_case() %> : <%= name.camel_case() %>Runtime.I<%= name.camel_case() %>Impl
    {
        public <%= name.camel_case() %>()
        {
            var _runtime = new <%= name.camel_case() %>Runtime.<%= name.camel_case() %>RuntimeComponent(this);
        }

        public void enable(IReadOnlyDictionary<string, string> propertyMap, <%= name.camel_case() %>Runtime.IMethodResult oResult)
        {
            // implement this method in C# here
        }

        public void start(<%= name.camel_case() %>Runtime.IMethodResult oResult)
        {
            // implement this method in C# here
        }

        public void stop(<%= name.camel_case() %>Runtime.IMethodResult oResult)
        {
            // implement this method in C# here
        }

        public void disable(<%= name.camel_case() %>Runtime.IMethodResult oResult)
        {
            // implement this method in C# here
        }

        public void take(IReadOnlyDictionary<string, string> propertyMap, <%= name.camel_case() %>Runtime.IMethodResult oResult)
        {
            // implement this method in C# here
        }
    }
}

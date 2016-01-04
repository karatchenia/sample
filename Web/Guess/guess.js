'use strict';

if (null == window.MyCompany) {
    window.MyCompany = {};
}

MyCompany.Report = {};
MyCompany.Session = {};

MyCompany.GetRandom = function (minInclusive, maxExclusive) {
    var result = Math.random() * (maxExclusive - minInclusive) + minInclusive;
    return result;
};

MyCompany.GetRandomInt = function (minInclusive, maxExclusive) {
    var raw = MyCompany.GetRandom(minInclusive, maxExclusive);
    var result = Math.floor(raw);
    return result;
};

MyCompany.RunAnync = function (callback, delay) {
    if (null == callback) {
        throw 'The "callback" must be not null in RunAnync.';
    }

    if (!delay) {
        delay = 0;
    }

    setTimeout(function () {
        callback();
    }, delay);
};

MyCompany.GetControl = function (controlId) {
    var result = (!controlId || !(controlId.length))
        ? null : document.getElementById(controlId);

    if (null == result) {
        var nameFormatted = !controlId ? 'null' : controlId;
        console.log('Error: cannot find the control named "'
            + nameFormatted + '".');
    }

    return result;
};

MyCompany.DecimalToString = function (decimalValue, decimalPlaces) {
    if (!decimalPlaces || decimalPlaces <= 0) {
        decimalPlaces = 2;
	}

	var precision = Math.pow(10, decimalPlaces);

    var result = parseFloat(Math.round(decimalValue * precision) / precision).toFixed(decimalPlaces);
    return result;
};

MyCompany.ScaleArray = function (values, valuesSize, multiplyCoefficient) {
    var j = 0;
    do {
        values[j] = multiplyCoefficient * values[j];
    }
    while (++j < valuesSize);
}

MyCompany.PreCalcRatiosAndMax = function (values, valuesSize, offset, multiplyCoefficient) {
    var ratios = [];

    var j = 0;
    var maxValue = 0;
    do {
        ratios[j] = multiplyCoefficient * values[j + offset];
        if (maxValue < ratios[j]) {
            maxValue = ratios[j];
        }
    }
    while (++j < valuesSize);

    var result = {};
    result.Ratios = ratios;
    result.MaxValue = maxValue;
    return result;
}

MyCompany.GetDistributionRatios = function (values, valuesSize, offset, totalCount, maxPercentage) {
    var multiplyCoefficient = !totalCount ? 0 : maxPercentage / totalCount;

    var preCalc = MyCompany.PreCalcRatiosAndMax(values, valuesSize, offset, multiplyCoefficient);
    var result = preCalc.Ratios;

    if (0 < preCalc.MaxValue) {
        var scale = maxPercentage / preCalc.MaxValue;
        MyCompany.ScaleArray(result, valuesSize, scale);
    }

    return result;
}

MyCompany.RatioToColor = function (ratio) {
	if (null == ratio){
		ratio = 0;
	}
	
	var isOk = 0 <= ratio && ratio <= 1;
	if (!isOk){
		throw 'The ratio(' + ratio + ') must be within [0, 1].';
	}
	
    var colorMax = 255;
    var color = (Math.floor(colorMax * ratio)).toString(16);
    var result = color + color + color;
    return result;
}

MyCompany.DrawDiagram = function (context, barCount, ratios, maxValue) {
    var j = 0;
    do {
        context.fillStyle = '#' + MyCompany.RatioToColor(j / barCount);

        var delta = maxValue - ratios[j];
        if (delta < 0) {//Rounding error.
            delta = 0;
        }

        var width = 30;
        context.fillRect(width * j, delta, width, ratios[j]);

        var text = '' + (MyCompany.MinimumId + j);
        var x = width * (j + 0.4);
        var y = maxValue + 20;
        context.fillText(text, x, y);
    }
    while (++j < barCount);
}

MyCompany.DrawGraph = function (graphControl) {
    if (0 == MyCompany.Report.Sessions) {
        return;
    }

    var ratios = MyCompany.GetDistributionRatios(MyCompany.Report.Distribution, MyCompany.ButtonCount,
		MyCompany.MinimumId, MyCompany.Report.Sessions, MyCompany.MaxPercentage);

    var context = graphControl.getContext('2d');
    context.clearRect(0, 0, graphControl.width, graphControl.height);
    context.font = MyCompany.Font;

    MyCompany.DrawDiagram(context, MyCompany.ButtonCount, ratios, MyCompany.MaxPercentage);
}

MyCompany.Report.CalcRatio = function () {
    var ratio = 0;
    if (0 < MyCompany.Report.TotalSteps) {
        ratio = MyCompany.DecimalToString(MyCompany.MaxPercentage *
        (MyCompany.Report.TotalSteps - MyCompany.Report.IncorrectSteps) / MyCompany.Report.TotalSteps);
    }

    return ratio;
}

MyCompany.Session.ReEnableButtons = function (previousNumber) {
    var className = 'shake';

    var j = 0;
    do {
        var button = MyCompany.Session.Buttons[j];
        button.classList.remove(className);

        if (previousNumber == MyCompany.MinimumId + j) {
            //Delay is required for the animation to work.
            var delayMs = 120;
            MyCompany.RunAnync(function () {
                var button = MyCompany.Session.Buttons[previousNumber - MyCompany.MinimumId];
                button.classList.add(className);
            }, delayMs);
        }

        button.disabled = false;
    }
    while (++j < MyCompany.ButtonCount);
}

MyCompany.Session.Start = function (progressControl, progressText, graphControl) {
    var ratio = MyCompany.Report.CalcRatio();
    progressControl.value = ratio;
    progressText.innerText = ratio;

    MyCompany.Session.Count = 0;

    var previousNumber = MyCompany.Session.NumberToGuess;
    MyCompany.Session.NumberToGuess = MyCompany.GetRandomInt(MyCompany.MinimumId, MyCompany.ButtonCount);

    MyCompany.Session.ReEnableButtons(previousNumber);
    MyCompany.DrawGraph(graphControl);
};

MyCompany.GetButtonId = function (button) {
    var key = button.accessKey;
    var buttonId = parseInt(key);
    if (buttonId < MyCompany.MinimumId || MyCompany.ButtonCount < buttonId) {
        throw 'Bad buttonId=' + buttonId;
    }

    return buttonId;
}

MyCompany.FinishSession = function() {
    MyCompany.Report.TotalSteps += MyCompany.ButtonCount - 1;
    MyCompany.Report.IncorrectSteps += MyCompany.Session.Count - 1;
    MyCompany.Report.Sessions++;

    if (MyCompany.Session.Count === 1) {
        MyCompany.Report.FirstStepCount++;
    }

    MyCompany.Report.Distribution[MyCompany.Session.Count]++;
}

MyCompany.FormHtmlReport = function () {
    var sessions = MyCompany.Report.Sessions;
    var firstPercent = (0 == sessions) ? '0' : MyCompany.DecimalToString(MyCompany.MaxPercentage *
	MyCompany.Report.FirstStepCount / sessions);

    var result = 'Last attempt: ' + MyCompany.Session.Count +
    ' out of ' + MyCompany.ButtonCount + ' steps.' +

    '<br>One step sessions: <strong>' + MyCompany.Report.FirstStepCount +
    ', ' + firstPercent + '%' +
    '</strong>.' +

    '<br>Total sessions: ' + MyCompany.Report.Sessions + '.';

    return result;
}

MyCompany.ReportAndRestartSession = function (progressControl, progressText, graphControl, runningTimeText) {
    runningTimeText.innerHTML = MyCompany.FormHtmlReport();

    MyCompany.RunAnync(function () {
        MyCompany.Session.Start(progressControl, progressText, graphControl);
    });
}

MyCompany.OnButtonClick = function (progressControl, progressText, graphControl, runningTimeText) {
    var result = function () {
        try {
            var button = arguments[0].currentTarget;
            var buttonId = MyCompany.GetButtonId(button);
            button.disabled = true;

            MyCompany.Session.Count++;

            var isCorrect = MyCompany.Session.NumberToGuess == buttonId;
            var shallStart = MyCompany.ButtonCount <= MyCompany.Session.Count;

            var isOneObviousChoiceLeft = !isCorrect && (MyCompany.ButtonCount - 1) <= MyCompany.Session.Count;
            if (isOneObviousChoiceLeft) {
                MyCompany.Session.Count++;
                shallStart = true;
            }

            if (isCorrect || shallStart) {
                MyCompany.FinishSession();
                MyCompany.ReportAndRestartSession(progressControl, progressText, graphControl, runningTimeText);
            }
        }
        catch (ex) {
            console.log('Error in the button.Click: ' + ex);
        }
    };

    return result;
}

MyCompany.FillNumberArray = function(size, defaultValue) {
    var result = Array.apply(null, Array(size)).map(Number.prototype.valueOf, defaultValue);
    return result;
}

MyCompany.InitializeMembers = function() {
    MyCompany.MaxPercentage = 100;
    MyCompany.MinimumId = 1;
    MyCompany.ButtonCount = 9;
    MyCompany.Font = '20px Arial';

    MyCompany.Report.TotalSteps = 0;
    MyCompany.Report.IncorrectSteps = 0;
    MyCompany.Report.FirstStepCount = 0;
    MyCompany.Report.Sessions = 0;
    MyCompany.Report.Distribution = MyCompany.FillNumberArray(MyCompany.ButtonCount, 0);

    MyCompany.Session.Buttons = [];
    MyCompany.Session.Count = 0;
    MyCompany.Session.NumberToGuess = 0;
}

MyCompany.CreateElementForButton = function(id) {
    var element = document.createElement('button');
    element.id = 'id_' + id;
    element.type = 'button';
    element.className = 'text btn magnify';
    element.accessKey = '' + id;

    var s = document.createElement('span');
    s.innerHTML = '<span class="underline">' + id + '</span>';
    element.appendChild(s);
    return element;
}

MyCompany.AppendSeparator = function(element) {
    var separator = document.createElement('span');
    separator.innerHTML = ' ';
    element.appendChild(separator);
}

MyCompany.InitializeButtons = function (buttons, progressControl, progressText, graphControl, runningTimeText) {
    buttons.innerHTML = '';

    var j = 0;
    do {
        var id = MyCompany.MinimumId + j;

        var element = MyCompany.CreateElementForButton(id);
        element.onclick = MyCompany.OnButtonClick(progressControl, progressText, graphControl, runningTimeText);

        buttons.appendChild(element);
        MyCompany.Session.Buttons.push(element);

        var isNotLast = j < MyCompany.ButtonCount - 1;
        if (isNotLast) {
            MyCompany.AppendSeparator(buttons);
        }
    }
    while (++j < MyCompany.ButtonCount);
}

MyCompany.InitializeButtonPressed = function () {
    var onKeyPressName = 'keypress';

    var keyPressed = function (e) {
        var zeroKeyCode = 48;
        var oneKeyCode = zeroKeyCode + 1;
        var codeMax = zeroKeyCode + MyCompany.ButtonCount;

        var isDigit = e && onKeyPressName == e.type &&
			e.keyCode &&
			oneKeyCode <= e.keyCode && e.keyCode <= codeMax;

        if (isDigit) {
            var index = e.keyCode - oneKeyCode;
            MyCompany.Session.Buttons[index].click();
        }
    };

    var body = document.documentElement.lastElementChild;
    body.removeEventListener(onKeyPressName, keyPressed);
    body.addEventListener(onKeyPressName, keyPressed);
}

MyCompany.InitializeMembers();

MyCompany.InitializeControls = function () {
    var buttons = MyCompany.GetControl('buttons');
    var runningTimeText = MyCompany.GetControl('runningTimeText');
    var graphControl = MyCompany.GetControl('graph');
    var progressControl = MyCompany.GetControl('progress');
    var progressText = MyCompany.GetControl('progressText');

    if (null == buttons || null == runningTimeText || null == graphControl ||
		null == progressControl || null == progressText) {
        return;
    }

    MyCompany.InitializeButtons(buttons, progressControl, progressText, graphControl, runningTimeText);
    MyCompany.InitializeButtonPressed();

    runningTimeText.innerHTML = 'Click one of the buttons.';
    MyCompany.Session.Start(progressControl, progressText, graphControl);
};
'use strict';

if ('undefined' === typeof (window.MyCompany)) {
    window.MyCompany = {};
}

MyCompany.Report = {};
MyCompany.Session = {};

MyCompany.getRandom = function (minInclusive, maxExclusive) {
    var result = Math.random() * (maxExclusive - minInclusive) + minInclusive;
    return result;
};

MyCompany.getRandomInt = function (minInclusive, maxExclusive) {
    var raw = MyCompany.getRandom(minInclusive, maxExclusive);
    var result = Math.floor(raw);
    return result;
};

MyCompany.runAnync = function (callback, delay) {
	if ('undefined' === typeof (callback)) {
        throw new Error('The "callback" must be not null in runAnync.');
    }

    if (!delay) {
        delay = 0;
    }

    setTimeout(function () {
        callback();
    }, delay);
};

MyCompany.getControl = function (controlId) {
    var result = (!controlId || !(controlId.length)) ? undefined : document.getElementById(controlId);

	if ('undefined' === typeof (result)) {
        var nameFormatted = !controlId ? 'null' : controlId;
        console.log('Error: cannot find the control named "' + nameFormatted + '".');
    }

    return result;
};

MyCompany.decimalToString = function (decimalValue, decimalPlaces) {
    if (!decimalPlaces || decimalPlaces <= 0) {
        decimalPlaces = 2;
	}

	var precision = Math.pow(10, decimalPlaces);

    var result = parseFloat(Math.round(decimalValue * precision) / precision).toFixed(decimalPlaces);
    return result;
};

MyCompany.scaleArray = function (values, valuesSize, multiplyCoefficient) {
    var j = 0;
    do {
        values[j] = multiplyCoefficient * values[j];
		++j;
    }
    while (j < valuesSize);
};

MyCompany.preCalcRatiosAndMax = function (values, valuesSize, offset, multiplyCoefficient) {
    var ratios = [];

    var j = 0;
    var maxValue = 0;
    do {
        ratios[j] = multiplyCoefficient * values[j + offset];
        if (maxValue < ratios[j]) {
            maxValue = ratios[j];
        }
		
		++j;
    }
    while (j < valuesSize);

    var result = {};
    result.Ratios = ratios;
    result.MaxValue = maxValue;
    return result;
};

MyCompany.getDistributionRatios = function (values, valuesSize, offset, totalCount, maxPercentage) {
    var multiplyCoefficient = !totalCount ? 0 : maxPercentage / totalCount;

    var preCalc = MyCompany.preCalcRatiosAndMax(values, valuesSize, offset, multiplyCoefficient);
    var result = preCalc.Ratios;

    if (0 < preCalc.MaxValue) {
        var scale = maxPercentage / preCalc.MaxValue;
        MyCompany.scaleArray(result, valuesSize, scale);
    }

    return result;
};

MyCompany.ratioToColor = function (ratio) {
	if ('undefined' === typeof ratio){
		ratio = 0;
	}
	
	var isOk = 0 <= ratio && ratio <= 1;
	if (!isOk){
		throw new Error('The ratio(' + ratio + ') must be within [0, 1].');
	}
	
    var colorMax = 255;
    var color = (Math.floor(colorMax * ratio)).toString(16);
    var result = color + color + color;
    return result;
};

MyCompany.drawDiagram = function (context, barCount, ratios, maxValue) {
    var j = 0;
    do {
        context.fillStyle = '#' + MyCompany.ratioToColor(j / barCount);

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
		++j;
    }
    while (j < barCount);
};

MyCompany.drawGraph = function (graphControl) {
    if (0 === MyCompany.Report.Sessions) {
        return;
    }

    var ratios = MyCompany.getDistributionRatios(MyCompany.Report.Distribution, MyCompany.ButtonCount,
		MyCompany.MinimumId, MyCompany.Report.Sessions, MyCompany.MaxPercentage);

    var context = graphControl.getContext('2d');
    context.clearRect(0, 0, graphControl.width, graphControl.height);
    context.font = MyCompany.Font;

    MyCompany.drawDiagram(context, MyCompany.ButtonCount, ratios, MyCompany.MaxPercentage);
};

MyCompany.Report.calcRatio = function () {
    var ratio = 0;
    if (0 < MyCompany.Report.TotalSteps) {
        ratio = MyCompany.decimalToString(MyCompany.MaxPercentage *
        (MyCompany.Report.TotalSteps - MyCompany.Report.IncorrectSteps) / MyCompany.Report.TotalSteps);
    }

    return ratio;
};

MyCompany.Session.reEnableButtons = function (previousNumber) {
    var className = 'shake';

    var j = 0;
    do {
        var button = MyCompany.Session.Buttons[j];
        button.classList.remove(className);

        if (previousNumber == MyCompany.MinimumId + j) {
            //Delay is required for the animation to work.
            var delayMs = 120;
            MyCompany.runAnync(function () {
                var button = MyCompany.Session.Buttons[previousNumber - MyCompany.MinimumId];
                button.classList.add(className);
            }, delayMs);
        }

        button.disabled = false;
		++j;
    }
    while (j < MyCompany.ButtonCount);
};

MyCompany.Session.start = function (progressControl, progressText, graphControl) {
    var ratio = MyCompany.Report.calcRatio();
    progressControl.value = ratio;
    progressText.innerText = ratio;

    MyCompany.Session.Count = 0;

    var previousNumber = MyCompany.Session.NumberToGuess;
    MyCompany.Session.NumberToGuess = MyCompany.getRandomInt(MyCompany.MinimumId, MyCompany.ButtonCount);

    MyCompany.Session.reEnableButtons(previousNumber);
    MyCompany.drawGraph(graphControl);
};

MyCompany.getButtonId = function (button) {
    var key = button.accessKey;
    var buttonId = parseInt(key, 10);
    if (buttonId < MyCompany.MinimumId || MyCompany.ButtonCount < buttonId) {
        throw new Error('Bad buttonId=' + buttonId);
    }

    return buttonId;
};

MyCompany.finishSession = function() {
    MyCompany.Report.TotalSteps += MyCompany.ButtonCount - 1;
    MyCompany.Report.IncorrectSteps += MyCompany.Session.Count - 1;
    MyCompany.Report.Sessions++;

    if (MyCompany.Session.Count === 1) {
        MyCompany.Report.FirstStepCount++;
    }

    MyCompany.Report.Distribution[MyCompany.Session.Count]++;
};

MyCompany.formHtmlReport = function () {
    var sessions = MyCompany.Report.Sessions;
    var firstPercent = (0 === sessions) ? '0' : MyCompany.decimalToString(MyCompany.MaxPercentage *
	MyCompany.Report.FirstStepCount / sessions);

    var result = 'Last attempt: ' + MyCompany.Session.Count +
    ' out of ' + MyCompany.ButtonCount + ' steps.' +

    '<br>One step sessions: <strong>' + MyCompany.Report.FirstStepCount +
    ', ' + firstPercent + '%' +
    '</strong>.' +

    '<br>Total sessions: ' + MyCompany.Report.Sessions + '.';

    return result;
};

MyCompany.reportAndRestartSession = function (progressControl, progressText, graphControl, runningTimeText) {
    runningTimeText.innerHTML = MyCompany.formHtmlReport();

    MyCompany.runAnync(function () {
        MyCompany.Session.start(progressControl, progressText, graphControl);
    });
};

MyCompany.onButtonClick = function (progressControl, progressText, graphControl, runningTimeText) {
    var result = function () {
        try {
            var button = arguments[0].currentTarget;
            var buttonId = MyCompany.getButtonId(button);
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
                MyCompany.finishSession();
                MyCompany.reportAndRestartSession(progressControl, progressText, graphControl, runningTimeText);
            }
        }
        catch (ex) {
            console.log('Error in the button.Click: ' + ex);
        }
    };

    return result;
};

MyCompany.fillNumberArray = function(size, defaultValue) {
    var result = Array.apply(null, Array(size)).map(Number.prototype.valueOf, defaultValue);
    return result;
};

MyCompany.initializeMembers = function() {
    MyCompany.MaxPercentage = 100;
    MyCompany.MinimumId = 1;
    MyCompany.ButtonCount = 9;
    MyCompany.Font = '20px Arial';

    MyCompany.Report.TotalSteps = 0;
    MyCompany.Report.IncorrectSteps = 0;
    MyCompany.Report.FirstStepCount = 0;
    MyCompany.Report.Sessions = 0;
    MyCompany.Report.Distribution = MyCompany.fillNumberArray(MyCompany.ButtonCount, 0);

    MyCompany.Session.Buttons = [];
    MyCompany.Session.Count = 0;
    MyCompany.Session.NumberToGuess = 0;
};

MyCompany.createElementForButton = function(id) {
    var element = document.createElement('button');
    element.id = 'id_' + id;
    element.type = 'button';
    element.className = 'text btn magnify';
    element.accessKey = '' + id;

    var s = document.createElement('span');
    s.innerHTML = '<span class="underline">' + id + '</span>';
    element.appendChild(s);
    return element;
};

MyCompany.appendSeparator = function(element) {
    var separator = document.createElement('span');
    separator.innerHTML = ' ';
    element.appendChild(separator);
};

MyCompany.initializeButtons = function (buttons, progressControl, progressText, graphControl, runningTimeText) {
    buttons.innerHTML = '';

    var j = 0;
    do {
        var id = MyCompany.MinimumId + j;

        var element = MyCompany.createElementForButton(id);
        element.onclick = MyCompany.onButtonClick(progressControl, progressText, graphControl, runningTimeText);

        buttons.appendChild(element);
        MyCompany.Session.Buttons.push(element);

        var isNotLast = j < MyCompany.ButtonCount - 1;
        if (isNotLast) {
            MyCompany.appendSeparator(buttons);
        }
		
		++j;
    }
    while (j < MyCompany.ButtonCount);
};

MyCompany.initializeButtonPressed = function () {
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
};

MyCompany.initializeMembers();

MyCompany.initializeControls = function () {
    var buttons = MyCompany.getControl('buttons');
    var runningTimeText = MyCompany.getControl('runningTimeText');
    var graphControl = MyCompany.getControl('graph');
    var progressControl = MyCompany.getControl('progress');
    var progressText = MyCompany.getControl('progressText');

    if ('undefined' == typeof buttons || 'undefined' == typeof runningTimeText ||
		'undefined' == typeof graphControl ||
		'undefined' == typeof progressControl || 'undefined' == typeof progressText) {
        return;
    }

    MyCompany.initializeButtons(buttons, progressControl, progressText, graphControl, runningTimeText);
    MyCompany.initializeButtonPressed();

    runningTimeText.innerHTML = 'Click one of the buttons.';
    MyCompany.Session.start(progressControl, progressText, graphControl);
};
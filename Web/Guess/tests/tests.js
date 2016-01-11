'use strict';

QUnit.test('getRandomInt', function(assert) {
	var minInclusive = 10;
	var maxExclusive = 12345678;

	var number = MyCompany.getRandomInt(minInclusive, maxExclusive);
	var type = typeof(number);
	assert.ok('number' === type, 'Returned type');
	
	var isWithin = minInclusive <= number && number < maxExclusive;
	assert.ok(isWithin, 'isWithin, number=' + number.toString() + '.');
});
QUnit.test('runAnync', function(assert) {
	assert.expect(1);
	var done = assert.async();

	var flag = false;
	
	var func = function(){
		assert.ok(!flag, 'Inside asynchronous function');
		done();
	};

	var delay = 1;
	MyCompany.runAnync(func, delay);
});
QUnit.test('getControl', function(assert) {
	var name = 'qunit';
	var control = MyCompany.getControl(name);
	assert.ok(null != control, 'The control "' + name + '" is not found in the html file.');
	assert.ok('DIV' === control.tagName.toUpperCase(), 'tagName');

	var type = typeof(control);
	assert.ok('object' === type, 'type');
});
QUnit.test('decimalToString', function(assert) {
	var value = 100.123421103;
	var decimalPlaces = 3;
	var actual = MyCompany.decimalToString(value, decimalPlaces);
	var expected = '100.123';
	assert.strictEqual(expected, actual);
});
QUnit.test('scaleArray', function(assert) {
	var values = [1, 4, 7];
	var multiplyCoefficient = 3;
	var expected = [3, 12, 21];

	var valuesSize = values.length;
	MyCompany.scaleArray(values, valuesSize, multiplyCoefficient);
	assert.deepEqual(expected, values);
});

//TODO: p3. move to a library.
var getDefaultSeparator = function(){
	var result = ' ';
	return result;
};
//TODO: p3. move to a library.
var assertArrayClose = function(assert, expected, actual, epsilon, message){
	if (expected === actual){
		return;
	}
	
	var separator = getDefaultSeparator();
	if (null == expected){
		assert.ok(null == actual, ['The actual must be null, but is:', actual, message].join(separator));
		return;
	}

	if (null == actual){
		assert.ok(false, ['The actual is null, but must be:', expected, message].join(separator));
		return;
	}
	
	var expectedLength = expected.length;
	assert.strictEqual('number', typeof(expectedLength), ['expected.length must be defined.', message].join(separator));

	var actualLength = actual.length;
	assert.strictEqual(expectedLength, actualLength, ['length', message].join(separator));

	var index = 0;
	do{
		var err = ['index=' + index.toString(), message].join(separator);
		assert.close(expected[index], actual[index], epsilon, err);
	}
	while(++index < expectedLength);
};

QUnit.test('getDistributionRatios', function(assert) {
	var values = [100, 8, 7, 4, 2, 1];
	var offset = 1;
	var valuesSize = values.length - offset;
	var maxPercentage = 100;
	var totalCount = values.reduce((prev, curr) => prev + curr) - values[0];
	var expected = [100, 87.5, 50, 25, 12.5];

	var actual = MyCompany.getDistributionRatios(values, valuesSize, offset, totalCount, maxPercentage);
	var epsilon = 0.000000001;
	assertArrayClose(assert,expected, actual, epsilon);
});
QUnit.test('ratioToColor', function(assert) {
	var tests = [
		{ratio:0, color:'000'},
		{ratio:1.0, color:'ffffff'},
		{ratio:0.5, color:'7f7f7f'},
		{ratio:0.3, color:'4c4c4c'}
	];

	for	(var index in tests){
		var actual = MyCompany.ratioToColor(tests[index].ratio);
		assert.strictEqual(tests[index].color, actual.toLowerCase());
	}
});
QUnit.test('formHtmlReport', function(assert) {
	MyCompany.initializeMembers();
	var expected = 'Last attempt: 0 out of 9 steps.<br>One step sessions: <strong>0, 0%</strong>.<br>Total sessions: 0.';
	var actual = MyCompany.formHtmlReport();
	assert.strictEqual(expected, actual, 'Empty report');
	
	try{
		MyCompany.Report.Sessions = 11;
		MyCompany.Report.FirstStepCount = 7;
		MyCompany.Session.Count = 3;
		
		var expected2 = 'Last attempt: 3 out of 9 steps.<br>One step sessions: <strong>7, 63.64%</strong>.<br>Total sessions: 11.';
		var actual2 = MyCompany.formHtmlReport();

		assert.strictEqual(expected2, actual2, 'Full report');
	}
	finally{
		MyCompany.initializeMembers();
	}
});
QUnit.test('fillNumberArray', function(assert) {
	var size = 7;
	var defaultValue = 1389;
	var expected = [defaultValue, defaultValue, defaultValue, defaultValue,
		defaultValue, defaultValue, defaultValue ];

	var actual = MyCompany.fillNumberArray(size, defaultValue);
    assert.deepEqual(expected, actual);
});
QUnit.test('drawDiagram', function(assert) {
	MyCompany.initializeMembers();
	try{
		var defaultValue = 3;
		MyCompany.Report.Distribution = MyCompany.fillNumberArray(MyCompany.ButtonCount, defaultValue);

		var someValue = 10;
		var someIndex = 4;
		MyCompany.Report.Distribution[someIndex] = someValue;

		MyCompany.Report.Sessions = defaultValue * (MyCompany.ButtonCount - 1) + someValue;

		var canvas = document.createElement('canvas');
		canvas.width = 300;
		canvas.height = 200;
		MyCompany.drawGraph(canvas);
		
		var context = canvas.getContext('2d');
		var imageData = context.getImageData(0, 0, canvas.width, canvas.height);
		assert.ok(null != imageData, 'imageData must be not null.');
		assert.strictEqual(canvas.width, imageData.width, 'width');
		assert.strictEqual(canvas.height, imageData.height, 'height');
		
		//TODO: p2. Compare the image using FileReader.
		//var coef = 4;
		//var dataSize = coef * canvas.width * canvas.height;
		var a = imageData.data;
		for(var i = 360; i <= 362; ++i){
			assert.equal(85, a[i], 'image[' + i.toString() + ']');
		}
		
		i = 363;
		assert.equal(255, a[i], 'image[' + i.toString() + ']');
	}
	finally{
		MyCompany.initializeMembers();
	}
});
function Convert-ToPSSafeString {
	param (
		[Parameter(Mandatory = $true)]
		[string]$InputString
	)
	if ($InputString -notmatch "'") {
		return "`"$InputString`""
	}
	$escaped = $InputString -replace '([`"$])', '`$1'
	return "`"$escaped`""
}

function main{ 
	param($program, $args);
	$joined = ($args | ForEach-Object { Convert-ToPSSafeString $_ }) -join " "
	Start-Process -FilePath "$program" -ArgumentList $joined -Verb "runAs" -Wait -WindowStyle "Hidden"
}

main
